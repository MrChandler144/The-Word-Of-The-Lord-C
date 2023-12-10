#define _CRT_SECURE_NO_DEPRECATE
#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PASS_RATE 97
#define TEXT_FILE "reference.txt"

// day to review | days between reviews (frequency) | prompt | verse

void ReplaceDate(FILE* file, int line_number, int days_between) {
  rewind(file);
  int current_line = 0;
  char buffer[1000];

  // calculate the new review date and put it into a string
  char new_review_date[6];
  int today = (int)time(NULL) / 86400;
  int day_to_review = today + days_between;
  sprintf(new_review_date, "%05d", day_to_review);

  // put the string into the file
  while (fgets(buffer, sizeof(buffer), file) != NULL) {
    current_line++;

    if (current_line == line_number) {
      // rewrite the first number
      long length = (long)strlen(buffer);
      fseek(file, length * -1, SEEK_CUR);
      fseek(file, -1, SEEK_CUR);
      // for some reason I can't combine those two lines
      fputs(new_review_date, file);
      fseek(file, 0, SEEK_END);
    } else {
      // this is not the correct line, leave it unchanged
      fputs(buffer, file);
    }
  }
}

void ReplaceFrequency(FILE* file, int line_number, int new_days_between) {
  rewind(file);
  int current_line = 0;
  char buffer[1000];

  // write the new frequency into a string
  char new_days_between_chars[5];
  sprintf(new_days_between_chars, "%04d", new_days_between);

  // write the string into the file
  while (fgets(buffer, sizeof(buffer), file) != NULL) {
    current_line++;

    if (current_line == line_number) {
      // rewrite the second number
      long length = (long)strlen(buffer);
      fseek(file, length * -1, SEEK_CUR);
      fseek(file, 5, SEEK_CUR);
      // for some reason I can't combine those two lines
      fputs(new_days_between_chars, file);
      fseek(file, 0, SEEK_END);
    } else {
      // this is not the correct line, leave it unchanged
      fputs(buffer, file);
    }
  }
}

int GetNewDaysBetween(int days_between) {
  // a big switch statement
  // takes in the old review frequency and returns the new one

  switch (days_between) {
    case 0:
      return 1;
    case 1:
      return 2;
    case 2:
      return 3;
    case 3:
      return 4;
    case 4:
      return 5;
    case 5:
      return 6;
    case 6:
      return 7;
    case 7:
      return 10;
    case 10:
      return 12;
    case 12:
      return 15;
    case 15:
      return 20;
    case 20:
      return 30;
    case 30:
      return 40;
    case 40:
      return 50;
    case 50:
      return 70;
    case 70:
      return 100;
    case 100:
      return 120;
    case 120:
      return 150;
    case 150:
      return 200;
    case 200:
      return 250;
    case 250:
      return 300;
    case 300:
      return 350;
    case 350:
      // delay by a year, accomodating leap years.
      // the 365 day period from March 1 to the Feb 28 before a leap day
      // should have a 366 day delay, otherwise just a 365 day delay
      time_t t = time(NULL);
      long time_long = (long)t;         // convert to a long for maths
      time_long = time_long - 5097600;  // subtract (31+28) days of seconds
      t = (time_t)time_long;            // convert back to a time_t object
      struct tm the_time = *localtime(&t);
      int year = the_time.tm_year + 1900;

      // if the adjusted year is one year before a leap year, return 366
      if ((year + 1 % 400 == 0) ||
          ((year + 1 % 4 == 0) && (year + 1 % 100 != 0))) {
        return 366;
      }
      return 365;
    case 365:
      return 400;
    case 366:
      return 400;
    case 400:
      return 450;
    case 450:
      return 500;
    case 500:
      return 550;
    case 550:
      return 600;
    case 600:
      return 650;
    case 650:
      return 700;
    case 700:
      return 750;
    case 750:
      return 800;
    case 800:
      return 850;
    case 850:
      return 900;
    case 900:
      return 950;
    case 950:
      return 1000;
    case 1000:
      return 1000;
    default:
      return 0;
  }
}

int GetNextIndex(const char* verse, int index) {
  // we want to be typing only every number, and every character after a space
  // returns the index of this new character

  int new_index = 0;
  // new index is only assigned when we find a valid value for it

  while (new_index == 0) {
    index++;

    // check if you've hit the end of the file
    if (verse[index] == '\0') {
      new_index = index;
    }

    // check if it's a number
    if (((int)verse[index] > 47) && ((int)verse[index] < 58)) {
      new_index = index;
    }

    // check if it's after a space
    if (verse[index - 1] == ' ') {
      new_index = index;
    }
  }

  return new_index;
}

int AreTheSame(int letter, int reference) {
  // a function that checks if you hit the right key
  // case 1: you hit the right key
  if (letter == reference) {
    return 1;
  }

  // case 2: you didn't hold shift
  if ((letter - reference == 32) && (letter > 96) && (letter < 123)) {
    return 1;
  }

  // case 3: you had caps lock on
  if ((letter - reference == -32) && (reference > 96) && (reference < 123)) {
    return 1;
  }

  // case 4: you hit the wrong key
  return 0;
}

void ClearScreen() {
  // credit to https://www.a1k0n.net/2011/07/20/donut-math.html
  printf("\x1b[2J");
  printf("\x1b[H");
}

int DoTheMemoryApp(const char* prompt, const char* verse) {
  // the core functionality - gets the user to type in the verse word by word
  // returns the percentage they got correct

  // initialise variables
  int letter = 100;
  int index = 0;
  int num_right = 0;
  int num_total = 0;
  int i = 0;

  // find the length of the line
  int length = strlen(verse);

  // show the prompt
  printf("%s\n", prompt);

  while (index < length) {
    // detect when the user hits a key
    if (_kbhit()) {
      // detect what key was hit
      letter = _getch();

      // check if the correct letter hit, and print the word
      int new_index = GetNextIndex(verse, index);
      if (AreTheSame(letter, (int)verse[index]) == 1) {
        num_right++;
        // print the word as it is
        for (i = index; i < new_index; i++) {
          printf("%c", verse[i]);
        }
      } else {
        new_index = GetNextIndex(verse, index);
        // print the word in uppercase
        for (i = index; i < new_index; i++) {
          printf("%c", (char)toupper(verse[i]));
        }
      }
      index = new_index;

      num_total++;
    }
  }

  printf("\n");

  // determine the percentage correct
  int score = (100 * num_right) / num_total;
  if (score < PASS_RATE) {
    // can't have you cheating
    ClearScreen();
  }

  printf("%d%%\n\n", score);
  return score;
}

void ReviewVerse(FILE* file, int line_number, int days_between,
                 const char* prompt, const char* verse) {
  int success_rate = 0;

  // review the verse until we get it right
  while (success_rate < PASS_RATE) {
    success_rate = DoTheMemoryApp(prompt, verse);
  }

  // once we pass update the review frequency and date
  days_between = GetNewDaysBetween(days_between);
  ReplaceFrequency(file, line_number, days_between);
  ReplaceDate(file, line_number, days_between);
}

int main(void) {
  // initialise some variables
  int day_to_review = 99;
  int days_between = 99;
  char prompt[50];
  char verse[500];

  int today = (int)time(NULL) / 86400;
  char buffer[1000];
  int current_line = 0;
  int times_run = 0;

  ClearScreen();

  // open the file
  FILE* fp;
  fp = fopen(TEXT_FILE, "r+");
  if (fp == NULL) {
    printf("Error opening file");
  }

  // read through the file
  while (fgets(buffer, sizeof(buffer), fp) != NULL) {
    current_line++;
    sscanf(buffer, "%d\t%d\t%[^\t\n]\t%[^\t\n]", &day_to_review, &days_between,
           prompt, verse);

    // everytime we finish reviewing a verse we reset back to the start
    // therefore counting the number of times we look at the first verse lets us
    // see how many verses we reviewed
    if (current_line == 1) {
      times_run++;
    }

    if (day_to_review <= today) {
      ReviewVerse(fp, current_line, days_between, prompt, verse);
      current_line = 0;
      rewind(fp);
    }
  }

  fclose(fp);

  // display the finish message
  if (times_run == 1) {
    printf(
        "No verses to review today! Maybe consider adding a new one to the "
        "list?\n");
  } else {
    printf("You've reviewed all your verses for today! Nice work!\n");
  }

  return 0;
}