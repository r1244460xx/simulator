from matplotlib import colors
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
filename = "brief_CP.csv"

with open(filename,"r") as f:
    titles = f.readline().split(",")
    times = f.readline().split(",")
    init_request = int(times[0])
    max_request = int(times[1])
    init_server = int(times[2])
    max_server = int(times[3])
    ssr_set = []
    usr_set = []
    dsr_set = []
    aeb_set = []
    aud_set = []
    ar_set = []
    for i in range(init_server, max_server):        
        cp_ssr = [] #satisfying service rate
        cp_usr = [] #unstatisfying service rate
        cp_dsr = [] #dropped service rate
        cp_aeb = [] #Average eMbb delay
        cp_aud = [] #Average uRLLC delay 
        cp_ar = [] #satisfying ratio
        cp_st = [] #satisfying thuput
        cp_it = [] #ideal thuput
        cp_sr = [] #demand sr
        cp_u = [] #sr utilization
        ld_ssr = [] #satisfying service rate
        ld_usr = [] #unstatisfying service rate
        ld_dsr = [] #dropped service rate
        ld_aeb = [] #Average eMbb delay
        ld_aud = [] #Average uRLLC delay 
        ld_ar = [] #satisfying ratio
        ld_st = [] #satisfying thuput
        ld_it = [] #ideal thuput
        ld_sr = [] #demand sr
        ld_u = [] #sr utilization
        x = []
        cp_y = []
        ld_y = []
        for j in range(init_request, max_request):
            CP_data = f.readline().split(",")
            cp_status = f.readline().split("\n")
            LD_data = f.readline().split(",")
            ld_status = f.readline().split("\n")
            cp_ssr.append(float(CP_data[0]))  
            cp_usr.append(float(CP_data[1]))  
            cp_dsr.append(float(CP_data[2]))  
            cp_aeb.append(float(CP_data[3]))  
            cp_aud.append(float(CP_data[4]))  
            cp_ar.append(float(CP_data[5]))
            cp_st.append(float(CP_data[6]))
            cp_it.append(float(CP_data[7]))
            cp_sr.append(float(CP_data[8]))
            cp_u.append(float(CP_data[9]))
            ld_ssr.append(float(LD_data[0]))  
            ld_usr.append(float(LD_data[1]))  
            ld_dsr.append(float(LD_data[2]))  
            ld_aeb.append(float(LD_data[3]))  
            ld_aud.append(float(LD_data[4]))  
            ld_ar.append(float(LD_data[5]))
            ld_st.append(float(LD_data[6]))
            ld_it.append(float(LD_data[7]))
            ld_sr.append(float(LD_data[8]))
            ld_u.append(float(LD_data[9]))
            x.append(j)
            cp_status = cp_status[0]
            ld_status = ld_status[0]
        requ_id = [k for k in range(0, max_request-1)]
        cp_c = 0
        ld_c = 0
        for k in cp_status:
            if k == "1":
                cp_y.append(1)
                cp_c += 1
            elif k == "0":
                cp_y.append(0)
        for k in ld_status:
            if k == "1":
                ld_y.append(1)
                ld_c += 1
            elif k == "0":
                ld_y.append(0)
        print(cp_c)
        print(ld_c)
        plt.figure()
        plt.plot(x, cp_ssr, x, ld_ssr)
        plt.title("SSR: "+str(i*4)+"/"+str(i))
        plt.figure()
        plt.plot(x, cp_aeb, x, ld_aeb)
        plt.title("AEB: "+str(i*4)+"/"+str(i))
        plt.figure()
        plt.plot(x, cp_aud, x, ld_aud)
        plt.title("AUD: "+str(i*4)+"/"+str(i))
        plt.figure()
        plt.plot(x, cp_ar, x, ld_ar)
        plt.title("AR: "+str(i*4)+"/"+str(i))  
        plt.figure()
        plt.plot(x, cp_st, x, ld_st)
        plt.title("ST: "+str(i*4)+"/"+str(i))
        plt.figure()
        plt.plot(x, cp_it, x, ld_it)
        plt.title("IT: "+str(i*4)+"/"+str(i))
        plt.figure()
        plt.plot(x, cp_sr, "o", x, ld_sr, "o")
        plt.title("SR: "+str(i*4)+"/"+str(i))
        plt.figure()
        plt.plot(x, cp_u, "o", x, ld_u, "o")
        plt.title("U: "+str(i*4)+"/"+str(i))
        """plt.figure()
        plt.subplot(2,1,1)
        plt.plot(requ_id, cp_y, "o", color='red')
        plt.title("Status: "+str(i*4)+"/"+str(i))
        plt.subplot(2,1,2)
        plt.plot(requ_id, ld_y, "o", color='black')
        plt.title("Status: "+str(i*4)+"/"+str(i))"""
        #plt.plot(x, cp_usr, x, ld_usr)
        #
        #plt.title("USR: "+str(i*4)+"/"+str(i))
          
        """ssr_set.append(ssr)
        usr_set.append(usr) 
        dsr_set.append(dsr)
        aeb_set.append(aeb)
        aud_set.append(aud)
        ar_set.append(ar)"""
    plt.show()
