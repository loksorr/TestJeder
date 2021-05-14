

    int available_characters = buffer_size();
    if (available_characters>30)
    {  
      char read_char = '0';
      while (read_char!=0x78)
      {
        read_char = buffer_1[inc_pointer('r', 1, 1)];
      }
      char new_counter = buffer_1[inc_pointer('r', 1, 1)];
      if (counter+1==new_counter || (counter==0xFF && new_counter==0x00))  //Chasing bits. Could work without it, but it's fine now. 
      {
        counter=new_counter;
        sucess_count++;
        sprintf(output, "%05X:%05X:%02X  %05X:%05X:%02X  ",sucess_count,old_values[0],counter,old_values[1],old_values[2],old_values[4]);
       
        //sprintf(output, tem, read_char, new_counter, counter, sucess_count);
        lcd_write_string(output);
        inc_pointer('r', 1, 27);
      }
      else
      {
        counter_error = counter-new_counter;
        old_values[4]=new_counter;
        counter=new_counter;
        //inc_pointer('r', 1, 2);        
        old_values[pos_out]=sucess_count;
        pos_out++;
        if(pos_out>3)
        {pos_out = 0;}
        sucess_count = 0;
        //char output[32];
        //sprintf(output, "%02X:%02X:%02X:%02X--   %02X:%02X:%02X:%02X:%02X  ", read_char, new_counter, counter, available_characters, buffer_1[inc_pointer('r', 1, 1)], buffer_1[inc_pointer('r', 1, 1)], buffer_1[inc_pointer('r', 1, 1)], buffer_1[inc_pointer('r', 1, 1)],buffer_1[inc_pointer('r', 1, 1)]);
        //lcd_write_string(output);
      }
      
    }