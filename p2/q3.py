#!/usr/local/bin/python3

ALPHA = 7/8
INITIAL_SRTT = 125

def new_srtt(old_srtt, alpha, r):
    return alpha*old_srtt + (1-alpha)*r

i = 0
srtt = INITIAL_SRTT

while(srtt > 100):
    print("Iteration " + str(i) + ": " + str(srtt))
    srtt = new_srtt(srtt, ALPHA, 80)
    i += 1

print("Iteration " + str(i) + ": " + str(srtt))

