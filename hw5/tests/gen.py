import random
import string
def random_string(num):
    return ''.join(random.choice(string.ascii_lowercase) for x in range(num))

fin = open("do.adtq","w")
fin.write("adtr 4 \n")
fin.write("adta -r 100000 \n")
fin.write("usage\n")
for i in range(100000):
    fin.write( "adtq "+random_string(4) + "\n" )
fin.write("usage"+"\n")
fin.close()
