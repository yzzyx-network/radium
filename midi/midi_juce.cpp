/* Copyright 2016 Kjetil S. Matheussen

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


/* This file is #include-d from ../audio/Juce_plugins.cpp */
//
// Note that compilation settings are a bit different here. FOR_LINUX/etc. are not defined, etc. */



#include "midi_ports_proc.h"

#include "OS_midi_proc.h"



namespace radium{
  
struct MidiOutputPortOs{
  std::unique_ptr<juce::MidiOutput> _midi_out;
};

  
struct MidiInputPortOs : juce::MidiInputCallback{
  std::unique_ptr<juce::MidiInput> _midi_input;
  const symbol_t *_port_name;
    
  MidiInputPortOs(const symbol_t *port_name)
    : _port_name(port_name)
  {}

  ~MidiInputPortOs(){
    if (_midi_input.get()!=NULL)
      _midi_input->stop();
    
    printf("_________________Deleted \"%s\"\n", _port_name->name);
    //getchar();
  }
    
  void handleIncomingMidiMessage(juce::MidiInput *source,
                                 const juce::MidiMessage &message 
                                 )
    override
  {

    /*
      int64_t message_ms = message.timeStamp;
      int64_t now_ms = Time::getMillisecondCounter();
    */

    const juce::uint8* raw = message.getRawData();
    int length = message.getRawDataSize();
      
    if(length==1)
      MIDI_InputMessageHasBeenReceived(_port_name, raw[0],0,0);
    else if(length==2)
      MIDI_InputMessageHasBeenReceived(_port_name, raw[0],raw[1],0);
    else if(length==3)
      MIDI_InputMessageHasBeenReceived(_port_name, raw[0],raw[1],raw[2]);
    else{
      R_ASSERT_NON_RELEASE(false);
    }
      
    //printf("got message to %s (%d %d %d)\n",(const char*)midi_input->getName().toUTF8(),(int)raw[0],(int)raw[1],(int)raw[2]);
  }
};
  
}



int MIDI_msg_len(uint32_t msg){

  int byte1 = MIDI_msg_byte1(msg);
  
  R_ASSERT(byte1!=0xf0);
  R_ASSERT(byte1!=0xf7);
  
  if (byte1<0x80 || byte1>0xff){
    RT_message("Illegal msg: %x",msg);
    return 0;
  }
  
  return juce::MidiMessage::getMessageLengthFromFirstByte(byte1);
}


void MIDI_OS_sendMessage(radium::MidiOutputPortOs *port,
                         int cc,
                         int data1,
                         int data2
                         )
{
  int len = juce::MidiMessage::getMessageLengthFromFirstByte(cc);

  /*
  {
    radium::MidiOutputPortOs *myport = static_cast<radium::MidiOutputPortOs*>(port);
    printf("Sending to %s. len: %d, (%d %d %d)\n",port==NULL?"Null":strdup(myport->midiout->getName().toUTF8()), len, cc, data1, data2);
  }
  */
  
  if(len==0){
    R_ASSERT_NON_RELEASE(false);
    return;
  }

  if(port==NULL){
    R_ASSERT_NON_RELEASE(false);
    return;
  }

#if 0
  if(true || time<0)
    printf("got midi. time: %f. startup_time: %f, jack time: %f. %x/%x/%x (%d/%d/%d)\n",
           ((double)time/(double)pc->pfreq),(float)startup_time,(float)RtMidiOut::getCurrentTime(myport->midiout->getCurrentApi()),
           cc,data1,data2,cc,data1,data2
           );
#endif

  juce::MidiOutput *output = port->_midi_out.get();

  if (output==NULL){// I.e. the "dummy" driver. (necessary on windows)
#if !defined(FOR_WINDOWS)
    R_ASSERT_NON_RELEASE(false);
#endif
    return;
  }

  
  //printf("current time: %f\n",(float)RtMidiOut::getCurrentTime(myport->midiout->getCurrentApi()));

  //printf("got midi: %x,%x,%x at time %f (rtmidi_time: %f) (current_time: %f)\n",cc,data1,data2,(float)time/(double)PFREQ,rtmidi_time,(float)RtMidiOut::getCurrentTime(midiout->getCurrentApi()));

  
  if(len==1)
    output->sendMessageNow(juce::MidiMessage(cc));
  else if(len==2)
    output->sendMessageNow(juce::MidiMessage(cc, data1));
  else
    output->sendMessageNow(juce::MidiMessage(cc, data1, data2));
}

static const char** string_array_to_char_array(const juce::StringArray &devices, int *retsize){
  *retsize = devices.size();
  
  const char **ret = (const char**)talloc(sizeof(const char*)*devices.size());
  
  for(int i=0 ; i < devices.size() ; i++)
    ret[i] = talloc_strdup(devices[i].toUTF8());

  return ret;
}

const char **MIDI_OS_getInputPortNames(int *retsize){ // returns all ports that's possible to connect to (whether we are connected or not)
  juce::StringArray devices = juce::MidiInput::getDevices();
  return string_array_to_char_array(devices, retsize);
}


const char **MIDI_OS_getOutputPortNames(int *retsize){ // returns all ports that's possible to connect to (whether we are connected or not)
  juce::StringArray devices = juce::MidiOutput::getDevices();
  return string_array_to_char_array(devices, retsize);
}

const char *MIDI_OS_getDefaultOutputPort(void){
  juce::StringArray devices = juce::MidiOutput::getDevices();
  if (devices.size()==0)
    return NULL;
  
  return talloc_strdup(devices[juce::MidiOutput::getDefaultDeviceIndex()].toUTF8());
}

const symbol_t *MIDI_OS_get_port_name(const char *name_c){
  juce::String name(name_c);

#if defined(FOR_LINUX)
  if (name.startsWith("Radium: "))
    name = name.substring(juce::String("Radium: ").length());
#endif

  return get_symbol(name.toUTF8());
}
                                        
radium::MidiOutputPortOs *MIDI_OS_create_output_port(const symbol_t *port_name, bool create_new_if_not_existing){

  juce::String name(port_name->name);

  juce::StringArray devices = juce::MidiOutput::getDevices();

  int device_id = devices.indexOf(name);

  /*
  for(int i=0;i<devices.size();i++){
    printf("AIAIAIAI. %d: %s (%d: %s)\n", i, talloc_strdup(devices[i].toUTF8()), device_id, name_c);
  }
  */

  if (device_id == -1 && !create_new_if_not_existing)
    return NULL;


  radium::MidiOutputPortOs *ret = new radium::MidiOutputPortOs;
  

  if (device_id>=0)
    //ret->_midi_out = juce::MidiOutput::openDevice(name);
    ret->_midi_out = juce::MidiOutput::openDevice(device_id);

#if !JUCE_WINDOWS
  else if (create_new_if_not_existing)
    ret->_midi_out = juce::MidiOutput::createNewDevice(name);
#endif

  if (ret->_midi_out.get()==NULL){
    
    if (create_new_if_not_existing)
      GFX_Message(NULL, "Error. Unable to open MIDI output device %s.\n", (const char*)name.toUTF8());
    
  }

  printf("midi output device opened. name: %s, device id: %d\n",(const char*)name.toUTF8(), device_id);

  return ret;
}

radium::MidiInputPortOs *MIDI_OS_create_input_port(const symbol_t *port_name, bool create_new_if_not_existing){
  juce::String name(port_name->name);
  
  juce::StringArray devices = juce::MidiInput::getDevices();

  int device_id = devices.indexOf(name);

  if (device_id==-1 && !create_new_if_not_existing)
    return NULL;

  /*
  for(int i=0;i<devices.size();i++){
    printf("%d: -%s- (%d: -%s- -%s-)\n", i, talloc_strdup(devices[i].toUTF8()), device_id, name_c, talloc_strdup(name.toUTF8()));
  }
  */
  
  radium::MidiInputPortOs *midi_input_callback = new radium::MidiInputPortOs(port_name);

  if (device_id>=0)
    midi_input_callback->_midi_input = juce::MidiInput::openDevice(device_id, midi_input_callback);
  
#if !JUCE_WINDOWS
  else if (create_new_if_not_existing)
    midi_input_callback->_midi_input = juce::MidiInput::createNewDevice(name, midi_input_callback);
#endif
  
  if (midi_input_callback->_midi_input.get()==NULL){
    
    if (create_new_if_not_existing)
      GFX_Message(NULL, "Error. Unable to open MIDI input device %s.\n", (const char*)name.toUTF8());
    
  } else {
    
    midi_input_callback->_midi_input->start();

  }

  return midi_input_callback;
}

void MIDI_OS_delete_input_port(radium::MidiInputPortOs *port){
  delete port;
}

void MIDI_OS_delete_output_port(radium::MidiOutputPortOs *port){
  delete port;
}

