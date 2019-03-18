#!/usr/local/bin/python3

ALPHA = 7/8
BETA = 3/4
RTT = 125
N_RTT = 500
""" How many iterations to establish stable values """
WARMUP = 100
""" How many iterations to run tests for? """
TEST = 20

def new_srtt(old_srtt, alpha, r):
    return alpha*old_srtt + (1-alpha)*r

def new_rttvar(old_rttvar, srtt, beta, r):
    return beta*old_rttvar + (1-beta) * abs(srtt-r)

def new_rto(srtt, rttvar):
    return srtt + (4 * rttvar)

def get_rtt(i, n, rtt, n_rtt):
    if (0 == i % n):
        return n_rtt 
    else:
        return rtt

srtt = RTT
rttvar = new_rttvar(RTT, srtt, BETA, RTT)
rto = new_rto(srtt, rttvar)

# Run simulation for increasing values of N
for n in range(1, 20):
    print("***** N = " + str(n) + " *****")
    print('{:8}{:14}{:14}{:14}{:14}'.format("N", "RTT", "SRTT", "RTT_VAR", "RTO"))
    for i in range(WARMUP + TEST):
        rtt = get_rtt(i, n, RTT, N_RTT)
        # Record the value of N when a timeout occurs
        # But only after warmup iterations have expired
        if (i >= WARMUP and rtt > rto):
            print(" ***** !!!TIMEOUT OCCURRED!!! (N = " + str(n) + ") ***** ")
        srtt = new_srtt(srtt, ALPHA, rtt)
        rttvar = new_rttvar(rttvar, srtt, BETA, rtt)
        rto = new_rto(srtt, rttvar)
        print('{:<8d}{:<14f}{:<14f}{:<14f}{:<14f}'.format(i, rtt, srtt, rttvar, rto))
