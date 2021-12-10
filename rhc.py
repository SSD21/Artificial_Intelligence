#AIPS1 task2 RHC
import random
import sys
import time

def func_eval(x, y):
    re = (1+((x+y+1)**2)*(19-14*x+(3*x**2)-14*y+6*x*y+(3*y**2)))*(30+((2*x-3*y)**2)*(18-32*x+(12*x**2)+4*y-36*x*y+(27*y**2)))
    return re


def RHC(sp, p, z):
    num_of_sol = 0
    neighbors = []
    points_arr = []
    #print(func_eval(0.4, -0.5))
    temp_sol = func_eval(sp[0], sp[1])
    flag = True
    #start = time.time()
    while flag:
        flag = False
        for i in range(p):
            z1 = random.uniform(-z, z)
            z2 = random.uniform(-z, z)
            temp_sp = (sp[0] + z1, sp[1] + z2)
            if -2 <= temp_sp[0] <= 2 and -2 <= temp_sp[1] <= 2:
                sol_1 = func_eval(temp_sp[0], temp_sp[1])
                points_arr.append(temp_sp)
                neighbors.append(sol_1)
        mn = min(neighbors)
        print('temp_sol: ', temp_sol)
        print('mn: ', mn)
        num_of_sol += 1
        if mn < temp_sol:  # return minimized solution
            temp_sol = mn
            sp = points_arr[neighbors.index(mn)]
            flag = True
    #end = time.time()
    return sp, temp_sol, num_of_sol #,(end-start)

    # temp_f = func_eval(sp[0],sp[1])
    # for i in range(p):
    #     z1 = random.uniform(-z,z)
    #     z2 = random.uniform(-z,z)
    #     if (sp[0] > 2) and (sp[1] >2):
    #         print("x,y values out of bounds")
    #         continue
    #     temp_f2 = func_eval(neighbors[0],neighbors[1])
    #     if temp_f2 < temp_f: #return minimized solution
    #         num_of_sol+=1
    # return neighbors[0],neighbors[1],temp_f2,num_of_sol



def main():
    sp = (0.4,-0.5)
    p = 120 
    z = 0.1 
    seed = 3 
    # sp = (1.9,-1.5)
    # p = 500
    # z = 1
    # seed = 100
    random.seed(seed)
    print(RHC(sp, p, z))


if __name__ == '__main__':
    main()




