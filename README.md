# Dictionary-Searching
Dictionary searching program for specific word formats usign multithreading

Searches for words within a dictionary in specified formats via command line. 

1) a1b2d3e => find all words that start with an ‘a’ followed by any single letter followed
by a ‘b’ followed by any two letters followed by a ‘d’ followed by any three letters
followed by an ‘e’. So matches would include: axbyzdjkle.

2) 0:30ing => all words whose length was in the range 3 .. 33 that end in “ing”

3) def0:30 => all words whose length was in the range 3 .. 33 that start with “def”

4) 3ie10:30ing => all words that have “ie” in character positions 4 & 5 (1 relative) then
have a sequence of 10 to 30 of any letters and end in “ing”. Word length will be in the
range 18 .. 38.

5) 3{ie}10:30ing => all words that have either an ‘i’ or an ‘e’ in character position 4 (1
relative) then have a sequence of 10 to 30 of any letters and end in “ing”. Word length
will be in the range 17 .. 37.
