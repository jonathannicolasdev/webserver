import os


files_in_dir = ';'.join(s for s in os.listdir('.') if (len(s) and s[0] != '.'))
print(files_in_dir)
