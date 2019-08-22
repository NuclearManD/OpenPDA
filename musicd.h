boolean musicd_running = false;

/*
  Mario Theme Song adapted from code written by: Dipto Pratyaksa

  Found at https://www.hackster.io/jrance/super-mario-theme-song-w-piezo-buzzer-and-arduino-1cc2e4
*/

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

uint16_t mario_sound_1[] = {   // mario main melody
  78,
  
  NOTE_E7, NOTE_E7, 0, NOTE_E7,
  0, NOTE_C7, NOTE_E7, 0,
  NOTE_G7, 0, 0,  0,
  NOTE_G6, 0, 0, 0,
  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,
  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0,
  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,
  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0,

  83, 83, 83, 83,
  83, 83, 83, 83,
  83, 83, 83, 83,
  83, 83, 83, 83,
  83, 83, 83, 83,
  83, 83, 83, 83,
  83, 83, 83, 83,
  83, 83, 83, 83,
  111, 111, 111, 83,
  83, 83, 83, 83,
  83, 83, 83, 83,
  83, 83, 83, 83,
  83, 83, 83, 83,
  83, 83, 83, 83,
  83, 83, 83, 83,
  83, 83, 83, 111,
  111, 111, 83, 83,
  83, 83, 83, 83,
  83, 83, 83, 83,
  83, 83
};

uint16_t mario_sound_2[] = {   // mario underworld melody
  56,

  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4, 0,
  0,
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4, 0,
  0,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, 0,
  0,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, 0,
  0, NOTE_DS4, NOTE_CS4, NOTE_D4,
  NOTE_CS4, NOTE_DS4,
  NOTE_DS4, NOTE_GS3,
  NOTE_G3, NOTE_CS4,
  NOTE_C4, NOTE_FS4, NOTE_F4, NOTE_E3, NOTE_AS4, NOTE_A4,
  NOTE_GS4, NOTE_DS4, NOTE_B3,
  NOTE_AS3, NOTE_A3, NOTE_GS3,
  0, 0, 0,

  83, 83, 83, 83,
  83, 83, 167,
  333,
  83, 83, 83, 83,
  83, 83, 167,
  333,
  83, 83, 83, 83,
  83, 83, 167,
  333,
  83, 83, 83, 83,
  83, 83, 167,
  167, 56, 56, 56,
  167, 167,
  167, 167,
  167, 167,
  56, 56, 56, 56, 56, 56,
  100, 100, 100,
  100, 100, 100,
  333, 333, 333
};

uint16_t* sound_sequences[] = {
  // structure:
  // {[length],(all tones),(all tone lengths in milliseconds)}
  
  mario_sound_1,
  mario_sound_2
};

uint8_t mario_composition[] = {3,0,1,1}; // Mario song, uses 3 sequences
uint8_t mario_single_composition[] = {1,0}; // Mario song simplified, uses 1 sequence
uint8_t* song_compositions[] = {
  
  // structure: {[num sequences], sequence1, sequence2, ... sequenceN}
  mario_composition,
  mario_single_composition
};
char* song_names[] = {"Mario"};

int music_select = 0;

void music_daemon(void* args){
  musicd_running = true;
  print("musicd: now playing '");
  print(song_names[music_select]);
  println("'");
  for(int sequence = 1;sequence<=song_compositions[music_select][0];sequence++){
    int sid = song_compositions[music_select][sequence];
    int seq_len = sound_sequences[sid][0];
    for(int i = 0;i<seq_len;i++){
      uint16_t note = sound_sequences[sid][i+1];
      uint16_t note_len = sound_sequences[sid][i+seq_len+1];
      if(!note)note=40000; // humans can't hear 40khz, so it will appear to not make sound.
      lcdTone(note);
      delay(note_len);
    }
  }
  println("musicd exiting...");
  musicd_running = false;
  vTaskDelete(NULL);
}

TaskHandle_t musicd_taskvar;
void start_music_daemon(int song){
  music_select = song;
  mkTask(music_daemon,"musicd", &musicd_taskvar, 1024);
}

void notify_user(){
  lcdTone(20);
  delay(80);
  lcdNoTone();
}
