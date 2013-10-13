/* Copyright 2000 Kjetil S. Matheussen

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. */






#include "nsmtracker.h"
#include "playerclass.h"
#include "PEQcommon_proc.h"
#include "PEQmempool_proc.h"
#include "PEQ_calc_proc.h"
#include "time_proc.h"
#include "patch_proc.h"
#include "scheduler_proc.h"

#include "PEQpitches_proc.h"

extern PlayerClass *pc;

// If patch max pitch is much lower than radium max pitch (MAX_PITCH), we avoid sending out two or more messages in a row with the same pitch value.
STime PEQ_CalcNextPitchEvent(
                                struct PEventQueue *peq,
                                STime time1,
                                STime time,
                                STime time2,
                                float x1,
                                float *x,
                                float x2
                                )
{
  int int_x;
  STime ntime=PEQ_CalcNextEvent(
                                time1,time,time2,
                                scale(x1, 0,128,0,INT_MAX),
                                &int_x,
                                scale(x2, 0,128,0,INT_MAX)
                                );
  *x = scale(int_x, 0,INT_MAX, 0.0,128.0); // convert back.
  return ntime;
}


void PE_ChangePitchFromStart(struct PEventQueue *peq,int doit);
void PE_ChangePitch(struct PEventQueue *peq,int doit);
void PE_ChangePitchToEnd(struct PEventQueue *peq,int doit);
void PE_ChangePitchFromStartToEnd(struct PEventQueue *peq,int doit);

void InitPEQpitches(
	struct Blocks *block,
	struct Tracks *track,
	struct Notes *note,
	int playlistaddpos
){
	float x;
	struct PEventQueue *peq;

	struct Pitches *pitch=note->pitches;

        if(track->patch==NULL)
          return;

	if(pitch==NULL && note->note == note->note_end)
          return;

	peq=GetPEQelement();
	peq->block=block;
	peq->track=track;
	peq->note=note;
	peq->time1=Place2STime(block,&note->l.p);

	if(pitch==NULL){
		peq->time2=Place2STime(block,&note->end);
		peq->TreatMe=PE_ChangePitchFromStartToEnd;
		PC_InsertElement(
			peq,playlistaddpos,
			PEQ_CalcNextPitchEvent(
                                                  peq,
                                                  peq->time1,
                                                  peq->time1,
                                                  peq->time2,
                                                  note->note,
                                                  &x,
                                                  note->note_end
			)
		);
		return;
	}

	peq->time2=Place2STime(block,&pitch->l.p);
	peq->TreatMe=PE_ChangePitchFromStart;
	peq->pitch=pitch;

	PC_InsertElement(
		peq,playlistaddpos,
		PEQ_CalcNextPitchEvent(
                                  peq,
                                  peq->time1,
                                  peq->time1,
                                  peq->time2,
                                  note->note,
                                  &x,
                                  pitch->note
		)
	);

}


static void scheduled_change_pitch(int64_t time, union SuperType *args){
  struct Tracks *track = args[0].pointer;
  struct Notes  *note  = args[1].pointer;
  float          x     = args[2].float_num;

  RT_PATCH_change_pitch(track->patch,
                           note->note,
                           x,
                           track,
                           time
                           );
}

static void SendPitchChange(float x,struct PEventQueue *peq){
	if(peq->track->patch!=NULL && peq->track->onoff==1){
          union SuperType args[3];
          args[0].pointer = peq->track;
          args[1].pointer = peq->note;
          args[2].float_num = x;

          SCHEDULER_add_event(peq->l.time, scheduled_change_pitch, &args[0], 3, SCHEDULER_ADDORDER_DOESNT_MATTER);

          /*
          RT_PATCH_change_pitch(peq->track->patch,
                                   peq->note->note,
                                   x,
                                   peq->track,
                                   peq->l.time
                                   );
          */
	}
}


void PE_ChangePitchFromStart(struct PEventQueue *peq,int doit){
	float x;
	STime ntime,btime;
	struct Pitches *next;

	btime=PC_TimeToRelBlockStart(pc->end_time);

	if(btime>=peq->time2){
		next=NextPitch(peq->pitch);
		peq->time1=peq->time2;
		if(next==NULL){
			peq->time2=Place2STime(peq->block,&peq->note->end);
			peq->TreatMe=PE_ChangePitchToEnd;
			PE_ChangePitchToEnd(peq,doit);
		}else{
			peq->nextpitch=next;
			peq->time2=Place2STime(peq->block,&next->l.p);
			peq->TreatMe=PE_ChangePitch;
			PE_ChangePitch(peq,doit);
		}
		return;
	}

	ntime=PEQ_CalcNextPitchEvent(
                                        peq,
		peq->time1,
		btime,
		peq->time2,
		peq->note->note,
		&x,
		peq->pitch->note
	);

	if(btime==ntime){
		Pdebug("btime==ntime, stopper, x: %d, btime: %d, x1: %d, x2: %d\n",x,btime,peq->note->note,peq->pitch->note);
		return;
	}

	if(ntime>peq->time2) ntime=peq->time2;

//	Pdebug("start->vel,Pitch: %d, time: %d, ntime: %d, btime: %d, time1: %d, time2: %d\n",x,time,ntime,btime,peq->time1,peq->time2);
	if(doit){
          SendPitchChange(x,peq);
	}

	PC_InsertElement(peq,0,ntime);

	return;
}



void PE_ChangePitch(struct PEventQueue *peq,int doit){
	float x;
	STime ntime,btime;
	struct Pitches *next;

	btime=PC_TimeToRelBlockStart(pc->end_time);

	if(btime>=peq->time2){
		next=NextPitch(peq->nextpitch);
		peq->time1=peq->time2;
		peq->pitch=peq->nextpitch;

		if(next==NULL){
			peq->time2=Place2STime(peq->block,&peq->note->end);
			peq->TreatMe=PE_ChangePitchToEnd;
			PE_ChangePitchToEnd(peq,doit);
		}else{
			peq->nextpitch=next;
			peq->time2=Place2STime(peq->block,&next->l.p);
			PE_ChangePitch(peq,doit);
		}
		return;
	}

	ntime=PEQ_CalcNextPitchEvent(
                peq,
		peq->time1,
		btime,
		peq->time2,
		peq->pitch->note,
		&x,
		peq->nextpitch->note
	);

	if(btime==ntime){
		Pdebug("Samme, stopper, x: %d\n",x);
		return;
	}

	if(ntime>peq->time2) ntime=peq->time2;

//	Pdebug("Player vel->vel, Pitch: %d, ntime: %d, btime: %d, time1: %d, time2: %d\n",x,ntime,btime,peq->time1,peq->time2);
	if(doit){
		SendPitchChange(x,peq);
	}

	PC_InsertElement(peq,0,ntime);

	return;

}

static float FindNextPitch(struct Notes *note){
  struct Notes *next_note = NextNote(note);
  if(next_note!=NULL)
    return next_note->note;
  else
    return 100;
}

void PE_ChangePitchToEnd(struct PEventQueue *peq,int doit){
	float x;
	STime ntime,btime;

	btime=PC_TimeToRelBlockStart(pc->end_time);

	if(btime>=peq->time2){
		ReturnPEQelement(peq);
		return;
	}

	ntime=PEQ_CalcNextPitchEvent(
                                     peq,
                                     peq->time1,
                                     btime,
                                     peq->time2,
                                     peq->pitch->note,
                                     &x,
                                     FindNextPitch(peq->pitch->note_note)
	);

	if(ntime>=peq->time2){
		ReturnPEQelement(peq);
		return;
	}

//	Pdebug("Player vel->end, Pitch: %d, ntime: %d, btime: %d, time1: %d, time2: %d\n",x,ntime,btime,peq->time1,peq->time2);
	if(doit){
		SendPitchChange(x,peq);
	}

	PC_InsertElement(peq,0,ntime);

	return;

}

void PE_ChangePitchFromStartToEnd(struct PEventQueue *peq,int doit){
	float x;
	STime ntime,btime;

	btime=PC_TimeToRelBlockStart(pc->end_time);

	if(btime>=peq->time2){
		ReturnPEQelement(peq);
		return;
	}

	ntime=PEQ_CalcNextPitchEvent(
                                        peq,
                                        peq->time1,
                                        btime,
                                        peq->time2,
                                        peq->note->note,
                                        &x,
                                        120 //peq->note->note_end+1		// Don't really know why I have to add 1, but it works...
	);

	if(ntime>=peq->time2){
		ReturnPEQelement(peq);
		return;
	}


//	Pdebug("Player start->end, Pitch: %d\n",x);
	if(doit){
		SendPitchChange(x,peq);
	}

	PC_InsertElement(peq,0,ntime);

	return;
}


