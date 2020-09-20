# -*- coding: utf-8 -*-
"""from sage.sat.solvers import cryptominisat"""
from sage.rings.polynomial.multi_polynomial_sequence import PolynomialSequence

BLK = int(input("BLK : "))#32
KEY = BLK-2
RND = int(input("RND : "))#1024
TST = int(input("TST : "))#16

CNST = (0,0,0,1,0,0,1,1,0,1,0,1,1,1,1, 1,1,1,0,1,1,0,0,1,0,1,0,0,0,0,1)

ofilename = "B"+str(BLK)+"R"+str(RND)+"T"+str(TST)+"_solver.sage"
ofile = open(ofilename,"w")

def elf_round(d,dy,k,ctr,cnst):
    v = dy[ctr]+(d[0]*d[1])+d[2]+k[ctr%KEY]+cnst[ctr%(BLK-1)]
    d[2] = dy[ctr]
    t = d[BLK-1]
    for i in range(BLK-1): d[BLK-i-1] = d[BLK-i-2]
    d[0] = t
    return v

var_names = []
for i in range(KEY) : var_names.append('k'+str(i))
for i in range(BLK) : var_names.append('x'+str(i))
for i in range(RND) : var_names.append('y'+str(i))

R = BooleanPolynomialRing(RND+BLK+KEY, var_names)
rgens = list(R.gens())

K = rgens[:KEY]
X = rgens[KEY:KEY+BLK]
Y = rgens[KEY+BLK:]


K_str = ""
for i in range(KEY) : K_str += (str(K[i])+",")
#K_str += str(K[KEY-1])
X_str = ""
for i in range(BLK) : X_str += (str(X[i])+",")
#X_str += str(X[BLK-1])
Y_str = ""
for i in range(RND-1) : Y_str += (str(Y[i])+",")
Y_str += str(Y[RND-1])

printable = """from sage.rings.polynomial.multi_polynomial_sequence import PolynomialSequence
from sage.sat.solvers import cryptominisat\n\n"""
printable += "BLK = "+str(BLK)+"\n"
printable += "KEY = "+str(KEY)+"\n"
printable += "RND = "+str(RND)+"\n"
printable += "TST = "+str(TST)+"\n\n"

printable += """
CNST = (0,0,0,1,0,0,1,1,0,1,0,1,1,1,1, 1,1,1,0,1,1,0,0,1,0,1,0,0,0,0,1)

def elf_enc(p,k,cc=CNST) :
    c = []
    for i in range(len(p)) : c.append(p[i])
    for i in range(RND) : 
		c[2] = (c[0]*c[1]+c[2]+k[i%KEY]+cc[i%(BLK-1)])%2
		t = c[BLK-1]
		for j in range(BLK-1) : c[BLK-j-1] = c[BLK-j-2]
		c[0] = t
    return c

"""

printable += "R.<"+K_str+X_str+Y_str+"> = BooleanPolynomialRing()\n\n"

my_system = Ideal(R,[])

for i in range(RND) : 
    t = elf_round(X,Y,K,i,CNST)
    my_system += t

Z = my_system.gens()

printable += "eqns = ["
for i in range(len(Z)-2) : printable += str(Z[i+1])+",\n        "
printable += str(Z[len(Z)-1])+"]\n"

last_line = ""

for i in range(BLK-3): 
	last_line += "y"+str(RND-1-i)+"=C["+str(i+3)+"],"

last_line += "y"+str(RND-BLK+2)+"=C["+str(0)+"],"
last_line += "y"+str(RND-BLK+1)+"=C["+str(1)+"],"
last_line += "y"+str(RND-BLK)+"=C["+str(2)+"]))"

printable += """
num_solutions = []
times = []

for v in range(TST):
    mkey = []
    P = []
    for i in range(KEY) : mkey.append(randrange(0,10)&1)
    for i in range(BLK) : P.append(randrange(0,10)&1)
    C = elf_enc(P,mkey,CNST)
    
    eqns_derived = []
    for i in range(len(eqns)) : eqns_derived.append(eqns[i].subs(x0=P[0],x1=P[1],x2=P[2],x3=P[3],x4=P[4],x5=P[5],x6=P[6],x7=P[7],"""

printable += last_line

printable += """
\n\n    S = PolynomialSequence(eqns_derived);
    strt = cputime()
    A = S.solve(n=Infinity, algorithm='sat', eliminate_linear_variables=false);
    elapsed = cputime(strt)
    times.append(elapsed)
    print \"elapsed \",v,\" : \",elapsed
    if A == false : num_solutions.append(0)
    else : num_solutions.append(len(A))\n\n
"""

####

printable += """
ofilename = \"B\"+str(BLK)+\"R\"+str(RND)+\"T\"+str(TST)+\"_plot.py\"
ofile = open(ofilename,\"w\")\n
"""

printable += """
printable = \"\"\"import matplotlib.pyplot as plt
num_solutions = (\"\"\"\n
"""

printable += """
for i in range(TST-1) : printable += str(num_solutions[i])+\",\"
printable += str(num_solutions[TST-1])+\")\\ntimes = (\"
for i in range(TST-1): printable += str(times[i])+\",\"
printable += str(times[TST-1])+\")\"
"""

printable += """
printable += \"\"\"
plt.subplot2grid((1,2), (0, 0), rowspan=1, colspan=1)
plt.hist(num_solutions)
plt.title(\\\"Avarage # of solutions : \\\"+str(float(sum(num_solutions))/len(num_solutions)))
plt.xlabel(\\\"number of solutions\\\")
plt.ylabel(\\\"Frequency\\\")

plt.subplot2grid((1,2), (0, 1), rowspan=1, colspan=1)
plt.hist(times)
plt.title(\\\"Average time : \\\"+str(float(sum(times)/len(times))))
plt.xlabel(\\\"Execution times\\\")
plt.ylabel(\\\"Frequency\\\")

plt.show()\\n
\"\"\"
"""

printable += """
ofile.write(printable)
ofile.close()\n
"""

#####

ofile.write(printable)
ofile.close()
