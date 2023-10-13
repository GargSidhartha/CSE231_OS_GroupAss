#!/bin/bash

# This is a Bash script with multiple commands.

# Command 1: Print a welcome message
echo "Welcome to my script!"

# Command 2: Display the current date and time
date

# Command 3: List files in the current directory
ls



# Command 5: Create a new directory
mkdir my_directory

# Command 6: Change to the new directory
cd my_directory

# Command 7: Print the current working directory
pwd

# Command 8: Create a new empty text file
touch new_file.txt

# Command 9: Append some text to the text file
echo "This is some text." >> new_file.txt

# Command 10: Display the contents of the text file
cat new_file.txt

# Command 11: Remove the new directory and its contents
cd ..
rm -r my_directory

# Command 12: Print a farewell message
echo "Script execution complete. Goodbye!"
