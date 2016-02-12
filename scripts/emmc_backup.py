import subprocess
import re
import xml.etree.ElementTree as ET
import copy

class XML:
	

	def __init__(self):
		self.child_name = "";
		self.size = "";
		self.start = ""
		self.partition = ""
		self.name = ""

	def setChildName(self, cName):
		self.child_name = cName;

	def setSize(self, sz):
		self.size = sz;

	def setStart(self, st):
		self.start = st;

	def setPartition(self, part):
		self.partition = part;

	def setName(self, n):
		self.name = n;

cmdQueue = {"start" : ["adb", "shell", "cat", "/sys/block/mmcblk0/{0}/start"],
        "size": ["adb", "shell", "cat", "/sys/block/mmcblk0/{0}/size"],
        "partition": ["adb", "shell", "cat", "/sys/block/mmcblk0/{0}/partition"],
        "pull": ["adb", "pull", "/dev/block/{0}", "{0}.img", ]}


fp = subprocess.check_output(["adb", "shell", "ls", "-l", "/dev/block/platform/msm_sdcc.1/by-name"])

blocks = re.findall("\w+ -> \S+", fp)
xmlElements = []

obj = XML();

for block in blocks:

	obj.setName(block.split()[0]);



	obj.setChildName(re.findall("mmcblk0p\d+", block.split()[2])[0]);
	tmpScript = cmdQueue['size'][3]
	
	cmdQueue['size'][3] = cmdQueue['size'][3].format(block)
	

	tmpScript1 = cmdQueue['pull'][2];
	tmpScript2 = cmdQueue['pull'][3];
	cmdQueue['pull'][2] = cmdQueue['pull'][2].format(obj.child_name);
	cmdQueue['pull'][3] = cmdQueue['pull'][3].format(obj.name);
	subprocess.check_output(cmdQueue['pull'])

	obj.setSize(subprocess.check_output(cmdQueue['size']).strip())
	cmdQueue['size'][3] = tmpScript;
	cmdQueue['pull'][2] = tmpScript1;
	cmdQueue['pull'][3] = tmpScript2;

	tmpScript = cmdQueue['start'][3]
	cmdQueue['start'][3] = cmdQueue['start'][3].format(obj.child_name)
	

	obj.setStart(subprocess.check_output(cmdQueue['start']).strip())
	
	cmdQueue['start'][3] = tmpScript;


	tmpScript = cmdQueue['partition'][3]
	
	cmdQueue['partition'][3] = cmdQueue['partition'][3].format(obj.child_name)
	

	obj.setPartition(subprocess.check_output(cmdQueue['partition']).replace("\n", ""))
	cmdQueue['partition'][3] = tmpScript;
	xmlElements.append(copy.deepcopy(obj));

	
	


print xmlElements[0].child_name;

root = ET.Element("data")


elements  = []

for element in xmlElements:
	ET.SubElement(root, "program", name=element.child_name, size=element.size, start=element.start)



tree = ET.ElementTree(root)
tree.write("partitions.xml")
	
