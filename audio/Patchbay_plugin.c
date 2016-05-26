/* Copyright 2013 Kjetil S. Matheussen

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


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "../common/nsmtracker.h"
#include "SoundPlugin.h"
#include "SoundPlugin_proc.h"

#include "SoundPluginRegistry_proc.h"

#define MAX_NUM_CHANNELS 32

typedef struct{
  int routes[MAX_NUM_CHANNELS*MAX_NUM_CHANNELS];
  bool output_is_nulled[MAX_NUM_CHANNELS]; // optimization to avoid cleaning clean memory. Works since SoundProducer supplies the same output buffers every time. 
} Data;



static void RT_process(SoundPlugin *plugin, int64_t time, int num_frames, float **inputs, float **outputs){
  //plugin =NULL; // crashreporter test.

  SoundPluginType *type = plugin->type;
  
  Data *data = (Data*)plugin->data;

  int out_ch;

  for(out_ch=0;out_ch<type->num_outputs;out_ch++){
    
    bool touched = false;
    int in_ch;
    float *out=outputs[out_ch];
    
    for(in_ch=0 ; in_ch<type->num_outputs ; in_ch++){

      int array_pos = in_ch*type->num_inputs + out_ch;
      
      int onoff = data->routes[array_pos];
      
      if(onoff==1){
        float *in=inputs[in_ch];
        
        if(touched==false){
          memcpy(out, in, sizeof(float)*num_frames);
          touched=true;
        }else{
          int i;
          for(i=0;i<num_frames;i++)
            out[i] += in[i];
        }

        data->output_is_nulled[out_ch] = false;
      }
      
    }
    
    if(touched==false && data->output_is_nulled[out_ch]==false) {
      memset(out,0,sizeof(float)*num_frames);
      data->output_is_nulled[out_ch] = true;
    }
  }
}

static void set_effect_value(SoundPlugin *plugin, int64_t time, int effect_num, float value, enum ValueFormat value_format, FX_when when){
  Data *data = (Data*)plugin->data;

  data->routes[effect_num] = value > 0.5f;
}

static float get_effect_value(struct SoundPlugin *plugin, int effect_num, enum ValueFormat value_format){
  Data *data = (Data*)plugin->data;
  return data->routes[effect_num];
}

static void *create_plugin_data(const SoundPluginType *plugin_type, struct SoundPlugin *plugin, hash_t *state, float sample_rate, int block_size){
  Data *data = V_calloc(1,sizeof(Data));
  data->routes[0]=1;
  data->routes[plugin_type->num_outputs+1]=1;
  for(int ch = 0 ; ch < MAX_NUM_CHANNELS ; ch++){
    data->output_is_nulled[ch] = true;
  }
  return data;
}

static void cleanup_plugin_data(SoundPlugin *plugin){
  V_free(plugin->data);
}

static const char *get_effect_name(struct SoundPlugin *plugin, int effect_num){
  SoundPluginType *type = plugin->type;
  static char names[MAX_NUM_CHANNELS*MAX_NUM_CHANNELS][64];
  int from = effect_num / type->num_inputs; // not sure if it's num_inputs or num_outputs here
  int to = effect_num % type->num_inputs; // same here
  sprintf(names[effect_num], "%d->%d",from,to);
  return names[effect_num];
}

static int get_effect_format(SoundPlugin *plugin, int effect_num){
  return EFFECT_FORMAT_BOOL;
}

static SoundPluginType plugin_type8 = {
 type_name                : "Patchbay",
 name                     : "Patchbay 8x8",
 info                     : "Sends any input channel to any output channel",
 num_inputs               : 8,
 num_outputs              : 8,
 is_instrument            : false,
 note_handling_is_RT      : false,
 num_effects              : 8*8,
 get_effect_format        : get_effect_format,
 get_effect_name          : get_effect_name,
 effect_is_RT             : NULL,
 create_plugin_data       : create_plugin_data,
 cleanup_plugin_data      : cleanup_plugin_data,

 RT_process       : RT_process,
 set_effect_value : set_effect_value,
 get_effect_value : get_effect_value,
 get_display_value_string : NULL,

 data                     : NULL
};

static SoundPluginType plugin_type32 = {
 type_name                : "Patchbay",
 name                     : "Patchbay 32x32",
 info                     : "Sends any input channel to any output channel",
 num_inputs               : 32,
 num_outputs              : 32,
 is_instrument            : false,
 note_handling_is_RT      : false,
 num_effects              : 32*32,
 get_effect_format        : get_effect_format,
 get_effect_name          : get_effect_name,
 effect_is_RT             : NULL,
 create_plugin_data       : create_plugin_data,
 cleanup_plugin_data      : cleanup_plugin_data,

 RT_process       : RT_process,
 set_effect_value : set_effect_value,
 get_effect_value : get_effect_value,
 get_display_value_string : NULL,

 data                     : NULL
};

void create_patchbay_plugin(void){
  PR_add_plugin_type(&plugin_type8);
  PR_add_plugin_type(&plugin_type32);
}
