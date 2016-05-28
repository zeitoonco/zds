import sys
import os
from formatter import parseList


def scanDirForLists(directory):
	dirList = os.listdir(directory)
	if '_list.json' in dirList:
		print("-list found in :", directory)
		parseList(os.path.join(directory, '_list.json'), templates)
	for f in dirList:
		npath = os.path.join(directory, f)
		if os.path.isdir(npath):
			scanDirForLists(npath)


def loadTemplates():
	ts = {}
	tempPath = os.path.join(localPath, 'templates')
	# if not ('exceptionClassTemplate' in tempPathList and 'exceptionMacroTemplate' in tempPathList):
	#	print('Cant find Template files.')
	#	exit()
	for t in os.listdir(tempPath):
		ta = os.path.join(tempPath, t)
		if os.path.isfile(ta):
			ts[t] = open(ta).read()
	return ts


if len(sys.argv) != 2:
	print('Invalid number of arguments. Root path of ZDS project expected.')
	exit()
rootPath = os.path.abspath(sys.argv[1])
localPath = os.path.abspath(os.path.dirname(sys.argv[0]))
print('Root:', rootPath)
rootPathList = os.listdir(rootPath)
localPathList = os.listdir(localPath)
if not ('executables' in rootPathList):
	print('Cant find ''executables'' directory.')
	exit()
if not ('library' in rootPathList):
	print('Cant find ''library'' directory.')
	exit()
if not ('templates' in localPathList):
	print('Cant find ''templates'' directory.')
	exit()
templates = loadTemplates()

scanDirForLists(os.path.join(rootPath, 'executables'))
scanDirForLists(os.path.join(rootPath, 'library'))

print('\nDone.')
