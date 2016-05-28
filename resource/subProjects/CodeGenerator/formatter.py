import json
import os
import datetime


def fillTemplate(temp, param):
	res = temp
	for i in param:
		res = res.replace('$' + i, param[i])
	return res


def generateExceptionHeaderFromList(fileAddrs, namespace, excList, templates):
	# cls:  $name $title
	# mrc:  $name $namspaces,
	# file: $headerid, $namespaceztart, $namespaceend, $macro $class

	param = {"namespaceztart": "", "namespaceend": "", "namespaces": "", "class": "", "macro": "",
	         "datetime": str(datetime.datetime.today())}
	# extract namespace
	if len(namespace) > 0:
		namespaces = namespace.split('::')
		param["namspaces"] = namespace + "::"
		for i in namespaces:
			param["namespaceztart"] += "namespace " + i + " {\n"
			param["namespaceend"] += "} // " + i + "\n"

	param['headerid'] = param["namspaces"].replace("::", "_").upper() + \
	                    os.path.split(fileAddrs)[1].upper().replace(".", "_")

	for exc in excList:
		paraml = {'name': exc['name'], 'title': exc['title'], 'namspaces': param["namspaces"]}
		param['class'] += fillTemplate(templates['exceptionClassTemplate'], paraml) + "\n\n"
		param['macro'] += fillTemplate(templates['exceptionMacroTemplate'], paraml) + "\n"

	filestr = fillTemplate(templates['exceptionHeaderFileTemplate'], param)
	with open(fileAddrs, 'w') as dfile:
		dfile.truncate()
		dfile.write(filestr)
		dfile.close()


def parseList(eList, templates):
	print('--Parsing...')
	with open(eList, 'r') as content_file:
		j = json.load(content_file)

	# extract namespace
	namespace = ""
	if 'namespace' in j:
		namespace = j['namespace']
	if len(namespace) > 0:
		print("\t--namespace: "+ namespace)

	# check for exceptions
	if 'exceptions' in j and len(j['exceptions']) > 0:
		print("\t--Generating exceptions.")
		generateExceptionHeaderFromList(
			os.path.join(os.path.dirname(eList), 'exceptions.hpp'), namespace, j['exceptions'], templates)
