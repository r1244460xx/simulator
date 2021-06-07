import matplotlib.pyplot as plt
filename = "brief_CP.csv"

with open(filename,"r") as f:
    titles = f.readline().split(",")
    times = f.readline().split(",")
    init_request = int(times[0])
    max_request = int(times[1])
    init_server = int(times[2])
    max_server = int(times[3])
    # ssr_set = []
    # usr_set = []
    # dsr_set = []
    # aeb_set = []
    # aud_set = []
    # ar_set = []
    for i in range(init_server, max_server):        
        cp_ss = [] #satisfied services
        cp_us = [] #unsatisfied services
        cp_ds = [] #dropped services
        cp_aed = [] #Average eMbb delay
        cp_aud = [] #Average uRLLC delay
        cp_aedd = [] #Average eMBB demand delay
        cp_audd = [] #Average uRLLC demand delay 
        cp_tsp = [] #total system performance
        cp_tisp = [] #total ideal system performance
        cp_tssp = [] #total satisfied system performance
        cp_tusp = [] #total unsatisfied system performance
        cp_aap = [] #Average application performance
        ld_ss = [] #satisfied services
        ld_us = [] #unsatisfied services
        ld_ds = [] #dropped services
        ld_aed = [] #Average eMbb delay
        ld_aud = [] #Average uRLLC delay
        ld_aedd = [] #Average eMBB demand delay
        ld_audd = [] #Average uRLLC demand delay 
        ld_tsp = [] #total system performance
        ld_tisp = [] #total ideal system performance
        ld_tssp = [] #total satisfied system performance
        ld_tusp = [] #total unsatisfied system performance
        ld_aap = [] #Average application performance
        li_ss = [] #satisfied services
        li_us = [] #unsatisfied services
        li_ds = [] #dropped services
        li_aed = [] #Average eMbb delay
        li_aud = [] #Average uRLLC delay
        li_aedd = [] #Average eMBB demand delay
        li_audd = [] #Average uRLLC demand delay 
        li_tsp = [] #total system performance
        li_tisp = [] #total ideal system performance
        li_tssp = [] #total satisfied system performance
        li_tusp = [] #total unsatisfied system performance
        li_aap = [] #Average application performance
        x = []
        for j in range(init_request, max_request):
            CP_data = f.readline().split(",")
            cp_status = f.readline().split("\n")
            LD_data = f.readline().split(",")
            ld_status = f.readline().split("\n")
            LI_data = f.readline().split(",")
            li_status = f.readline().split("\n")

            cp_ss.append(float(CP_data[0]))
            cp_us.append(float(CP_data[1]))
            cp_ds.append(float(CP_data[2])) 
            cp_tsp.append(float(CP_data[3]))
            cp_tssp.append(float(CP_data[4]))
            cp_tusp.append(float(CP_data[5]))
            cp_tisp.append(float(CP_data[6]))
            cp_aed.append(float(CP_data[7]))
            cp_aedd.append(float(CP_data[8]))
            cp_aud.append(float(CP_data[9]))
            cp_audd.append(float(CP_data[10]))
            cp_aap.append(float(CP_data[11]))

            ld_ss.append(float(LD_data[0]))
            ld_us.append(float(LD_data[1]))
            ld_ds.append(float(LD_data[2]))
            ld_tsp.append(float(LD_data[3]))
            ld_tssp.append(float(LD_data[4]))
            ld_tusp.append(float(LD_data[5]))
            ld_tisp.append(float(LD_data[6]))
            ld_aed.append(float(LD_data[7]))
            ld_aedd.append(float(LD_data[8]))
            ld_aud.append(float(LD_data[9]))
            ld_audd.append(float(LD_data[10]))
            ld_aap.append(float(LD_data[11]))

            li_ss.append(float(LI_data[0]))
            li_us.append(float(LI_data[1]))
            li_ds.append(float(LI_data[2]))
            li_tsp.append(float(LI_data[3]))
            li_tssp.append(float(LI_data[4]))
            li_tusp.append(float(LI_data[5]))
            li_tisp.append(float(LI_data[6]))
            li_aed.append(float(LI_data[7]))
            li_aedd.append(float(LI_data[8]))
            li_aud.append(float(LI_data[9]))
            li_audd.append(float(LI_data[10]))
            li_aap.append(float(LI_data[11]))

            x.append(j)
            cp_status = cp_status[0]
            ld_status = ld_status[0]
            li_status = li_status[0]
        """requ_id = [k for k in range(0, max_request-1)]
        cp_c = 0
        ld_c = 0
        li_c = 0
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
        for k in li_status:
            if k == "1":
                li_y.append(1)
                li_c += 1
            elif k == "0":
                li_y.append(0)
        print(cp_c)
        print(ld_c)
        print(li_c)"""
        fontSize = 28
        if i == init_server or i == max_server-1:
            plt.rc('xtick', labelsize=20)
            plt.rc('ytick', labelsize=20)

            plt.figure()
            plt.plot(x, cp_ss, "rx", x, ld_ss, "g+", x, li_ss, "bo")
            plt.legend(["CPA", "LD", "LI"], loc=0, fontsize= 20)
            plt.xlabel("Number of service requests", fontsize=fontSize)
            plt.ylabel("Percentage of saatisfied services(%)", fontsize=fontSize)
            plt.title("Percentage of satisfied services to service requests, X=" + str(4*i) + ", Y=" + str(i), fontsize=fontSize)
            
            # plt.figure()
            # plt.plot(x, cp_us, "r", x, ld_us, "g", x, li_us, "b")
            # plt.legend(["CPA", "LD", "LI"], loc=0, fontsize= 20)
            # plt.xlabel("Number of service requests", fontsize=fontSize)
            # plt.ylabel("Percentage", fontsize=fontSize)
            # plt.title("Percentage of unsatisfied services to all ones: " + str(4 * i) + "MEC / " + str(i) + "CC", fontsize=fontSize)
            
            # plt.figure()
            # plt.plot(x, cp_ds, "r", x, ld_ds, "g", x, li_ds, "b")
            # plt.legend(["CPA", "LD", "LI"], loc=0, fontsize= 20)
            # plt.xlabel("Number of service requests", fontsize=fontSize)
            # plt.ylabel("Percentage", fontsize=fontSize)
            # plt.title("Percentage of dropped services to all ones: " + str(4 * i) + "MEC / " + str(i) + "CC", fontsize=fontSize)
            
            plt.figure()
            plt.plot(x, cp_aed, "rx", x, ld_aed, "g+", x, li_aed, "bo", x, cp_aedd, "y^")
            plt.legend(["CPA", "LD", "LI", "Required"], loc=0, fontsize= 20)
            plt.xlabel("Number of service requests", fontsize=fontSize)
            plt.ylabel("E2E delay(ms)", fontsize=fontSize)
            plt.title("Average eMBB E2E delay, X=" + str(4*i) + ", Y=" + str(i), fontsize=fontSize)
            
            plt.figure()
            plt.plot(x, cp_aud, "rx", x, ld_aud, "g+", x, li_aud, "bo", x, cp_audd, "y^")
            plt.legend(["CPA", "LD", "LI", "Required"], loc=0, fontsize= 20)
            plt.xlabel("Number of service requests", fontsize=fontSize)
            plt.ylabel("E2E delay(ms)", fontsize=fontSize)
            plt.title("Average uRLLC E2E delay, X=" + str(4*i) + ", Y=" + str(i), fontsize=fontSize)
        
            # plt.figure()
            # plt.plot(x, cp_tsp, "r", x, ld_tsp, "g", x, li_tsp, "b")
            # plt.legend(["CPA", "LD", "LI"], loc=0, fontsize= 20)
            # plt.xlabel("Number of service requests", fontsize=fontSize)
            # plt.ylabel("Percentage", fontsize=fontSize)
            # plt.title("Percentage of total system performance to ideal one " + str(4 * i) + "MEC / " + str(i) + "CC", fontsize=fontSize)
            
            # plt.figure()
            # plt.plot(x, cp_tisp, "r", x, ld_tisp, "g", x, li_tisp, "b")
            # plt.legend(["CPA", "LD", "LI"], loc=0, fontsize= 20)
            # plt.xlabel("Number of service requests", fontsize=fontSize)
            # plt.ylabel("Percentage", fontsize=fontSize)
            # plt.title("Percentage of total ideal system performance to ideal one " + str(4 * i) + "MEC / " + str(i) + "CC", fontsize=fontSize)
        

            # plt.figure()
            # plt.plot(x, cp_tssp, "rx", x, ld_tssp, "g+", x, li_tssp, "bo")
            # plt.legend(["CPA", "LD", "LI"], loc=0, fontsize= 20)
            # plt.xlabel("Number of service requests", fontsize=fontSize)
            # plt.ylabel("Percentage of total application of satisfied services to service requests(%)", fontsize=fontSize)
            # plt.title("Percentage of total application \nof satisfied services to service requests, X=" + str(4*i) + ", Y=" + str(i), fontsize=fontSize)
            
            # plt.figure()
            # plt.plot(x, cp_tusp, "r", x, ld_tusp, "g", x, li_tusp, "b")
            # plt.legend(["CPA", "LD", "LI"], loc=0, fontsize= 20)
            # plt.xlabel("Number of service requests", fontsize=fontSize)
            # plt.ylabel("Percentage", fontsize=fontSize)
            # plt.title("Percentage of total unsatisfied system performance to ideal one " + str(4 * i) + "MEC / " + str(i) + "CC", fontsize=fontSize)

            plt.figure()
            plt.plot(x, cp_aap, "rx", x, ld_aap, "g+", x, li_aap, "bo")
            plt.legend(["CPA", "LD", "LI"], loc=0, fontsize= 20)
            plt.xlabel("Number of service requests", fontsize=fontSize)
            plt.ylabel("Average percentage \nof normalized application performance(%)", fontsize=fontSize)
            plt.title("Average percentage of normalized application performance, X=" + str(4*i) + ", Y=" + str(i), fontsize=fontSize)

    plt.show()
