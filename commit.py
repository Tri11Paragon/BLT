#!/usr/bin/python3

import subprocess

#---------------------------------------
#		CONFIG
#---------------------------------------

VERSION_BEGIN_STR = "set(BLT_VERSION "
VERSION_END_STR = ")"
PATCH_LIMIT = 1000

#---------------------------------------
#	      DO NOT TOUCH
#---------------------------------------

def load_cmake():
	cmake_file = open("CMakeLists.txt", 'r')
	cmake_text = cmake_file.read()
	cmake_file.close()
	return cmake_text

def write_cmake(cmake_text):
	cmake_file = open("CMakeLists.txt", 'w')
	cmake_file.write(cmake_text)
	cmake_file.close()

def get_version(cmake_text):
	begin = cmake_text.find(VERSION_BEGIN_STR) + len(VERSION_BEGIN_STR)
	end = cmake_text.find(VERSION_END_STR, begin)
	return (cmake_text[begin:end], begin, end)

def split_version(cmake_text):
	version, begin, end = get_version(cmake_text)
	version_parts = version.split('.')
	return (version_parts, begin, end)

def recombine(cmake_text, version_parts, begin, end):
	constructed_version = version_parts[0] + '.' + version_parts[1] + '.' + version_parts[2]
	constructed_text_begin = cmake_text[0:begin]
	constrcuted_text_end = cmake_text[end::]
	return constructed_text_begin + constructed_version + constrcuted_text_end


def inc_major(cmake_text):
	version_parts, begin, end = split_version(cmake_text)
	version_parts[0] = str(int(version_parts[0]) + 1)
	version_parts[1] = '0'
	version_parts[2] = '0'
	return recombine(cmake_text, version_parts, begin, end)

def inc_minor(cmake_text):
	version_parts, begin, end = split_version(cmake_text)
	version_parts[1] = str(int(version_parts[1]) + 1)
	version_parts[2] = '0'
	return recombine(cmake_text, version_parts, begin, end)

def inc_patch(cmake_text):
	version_parts, begin, end = split_version(cmake_text)
	if int(version_parts[2]) + 1 >= PATCH_LIMIT:
		return inc_minor(cmake_text)
	version_parts[2] = str(int(version_parts[2]) + 1)
	return recombine(cmake_text, version_parts, begin, end)

cmake_text = load_cmake()
cmake_version = get_version(cmake_text)[0]
print(f"Current Version: {cmake_version}")

try:
	type = input("What kind of commit is this ((M)ajor, (m)inor, (p)atch)? ")

	if type.startswith('M'):
		print("Selected major")
		write_cmake(inc_major(cmake_text))
	elif type.startswith('m'):
		print("Selected minor")
		write_cmake(inc_minor(cmake_text))
	elif type.startswith('p') or type.startswith('P') or len(type) == 0:
		print("Selected patch")
		write_cmake(inc_patch(cmake_text))

	subprocess.call(["git", "add", "*"])
	subprocess.call(["git", "commit"])
	subprocess.call(["sh", "-c", "git remote | xargs -L1 git push --all"])
except KeyboardInterrupt:
	print("\nCancelling!")
