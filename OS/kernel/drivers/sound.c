//2016年07月07日20:07:07

#include <sound.h>
#include <timer.h>
#include <common.h>

static void play_sound(uint32_t nFrequence)
{
	uint32_t Div;
	uint8_t tmp;
	
	Div = 1193180 / nFrequence;
	outb(0x43, 0xb6);
	outb(0x42, (uint8_t) (Div));
	outb(0x42, (uint8_t) (Div >> 8));
	// 声音驱动
	tmp = inb(0x61);
	if(tmp != (tmp | 3))
	{
		outb(0x61, tmp | 3);
	}
}

// 关声音
static void nosound()
{
	uint8_t tmp = (inb(0x61) & 0xFC);
	outb(0x61, tmp);
}

void beep(int frequency, int milliseconds)
{
	play_sound(frequency);
	mSleep(milliseconds);
	nosound();
}

void playNote(char *note, int milliseconds)
{
	uint32_t frequency;

	if(*(note) == 'C') //user wants to play a C
  	{
    		if(*(note + 1) == '4')
    		{
      			if(*(note + 2) == '#')
      			{
        			frequency = 277;
      			}else{
        			frequency = 262;
      			}
    		}else if(*(note + 1) == '5')
    		{
      			if(*(note + 2) == '#')
		      	{
				frequency = 554;
		      	}else{
				frequency = 523;
		     	 }      
	    	}else if(*(note + 1) == '6')
	    	{
	      		if(*(note + 2) == '#')
	      		{
				frequency = 1109;
	      		}else{
				frequency = 1046;
	      		}      
	    	}

  	}else if(*(note) == 'D') //user wants to play a D
  	{
    		if(*(note + 1) == '4')
    		{
      			if(*(note + 2) == 'b')
		      	{
				frequency = 277;
		      	}else if(*(note + 2) == '#')
		      	{
				frequency = 311;
		      	}else{
				frequency = 294;
		      	}      
		}else if(*(note + 1) == '5')
		{
		      if(*(note + 2) == 'b')
		      {
				frequency = 554;
		      }else if(*(note + 2) == '#')
		      {
				frequency = 622;
		      }else{
				frequency = 587;
		      }
		}
  
		}else if(*(note) == 'E') //user wants to play a E
  		{
			if(*(note + 1) == '4')
		    	{
			      if(*(note + 2) == 'b')
				{
					frequency = 311;
		      		}else{
					frequency = 330;
		      		}      
		    	}else if(*(note + 1) == '5')
			{
		      		if(*(note + 2) == 'b')
				{
					frequency = 622;
		      		}else
		      		{
					frequency = 659;
		      		}      
		    	}

	  }else if(*(note) == 'F') //user wants to play a F
		  {
		    	if(*(note + 1) == '4')
		    	{
		      		if(*(note + 2) == '#')
		      		{
					frequency = 370;
		      		}else{
					frequency = 349;
		     		}      
		    	}else if(*(note + 1) == '5')
		    		{
		      		if(*(note + 2) == '#')
		      		{
					frequency = 740;
		      		}else{
				frequency = 698;
		      		}      
		    }
  
	}else if(*(note) == 'G') //user wants to play a G
	{
		if(*(note + 1) == '4')
		{
	      		if(*(note + 2) == 'b')
	      		{
				frequency = 370;
	      		}else if(*(note + 2) == '#')
	      		{
				frequency = 415;
	      		}else{
				frequency = 392;
	      		}      
	    	}else if(*(note + 1) == '5')
	    		{
	      		if(*(note + 2) == 'b')
	      		{
				frequency = 740;
	      		}else if(*(note + 2) == '#')
	      		{
				frequency = 831;
	      		}else{
				frequency = 784;
	      		}      
	    }

	  }else if(*(note) == 'A') //user wants to play a A
	  {
	  	if(*(note + 1) == '4')
	    	{
	      		if(*(note + 2) == 'b')
	      		{
				frequency = 415;
	      		}else if(*(note + 2) == '#')
	      		{
				frequency = 466;
	      		}else{
				frequency = 440;
	     		}      
	    	}else if(*(note + 1) == '5')
	    	{
	      		if(*(note + 2) == 'b')
	      		{
				frequency = 831;
	      		}else if(*(note + 2) == '#')
	      		{
				frequency = 932;
	      		}else{
				frequency = 880;
	      		}      
	    	}


  	}else if(*(note) == 'B') //user wants to play a B
  	{
    		if(*(note + 1) == '4')
    		{
      			if(*(note + 2) == 'b')
      			{
        			frequency = 466;
      			}else{
        			frequency = 494;
      			}      
    		}else if(*(note + 1) == '5')
    		{
      			if(*(note + 2) == 'b')
      		{
      		  	frequency = 932;
      			}else{
        		frequency = 988;
      			}      
    		}
  	}

  	play_sound(frequency);
 	mSleep(milliseconds);
 	nosound();
}


void song()
{
    
  uint32_t wholeNote = 1100;
  uint32_t half = wholeNote / 2, quarter = wholeNote / 4, eigth = wholeNote / 8, sixteenth = wholeNote / 16;

  //~ playNote("A5", eigth);
  playNote("B4", eigth);
  playNote("A5", eigth);
  playNote("F5#", eigth);
  playNote("E5b", eigth);

  playNote("A5", sixteenth);
  playNote("F5#", eigth);
  playNote("E5b", quarter);
  
  playNote("C5", eigth);
  playNote("C6", eigth);
  playNote("G5", eigth);
  playNote("E5", eigth);
  
  playNote("C6", sixteenth);
  playNote("G5", eigth);
  playNote("E5", quarter);
  
  playNote("B4", eigth);
  playNote("A5", eigth);
  playNote("F5#", eigth);
  playNote("E5b", eigth);

  playNote("A5", sixteenth);
  playNote("F5#", eigth);
  playNote("E5b", quarter);

  playNote("E5b", sixteenth);
  playNote("E5", sixteenth);
  playNote("F5", eigth);
  playNote("F5", sixteenth);
  playNote("F5#", sixteenth);
  playNote("G5", eigth);
  
  playNote("G5", sixteenth);
  playNote("A5b", sixteenth);
  playNote("A5", eigth);
  playNote("B5", quarter);
  

}
