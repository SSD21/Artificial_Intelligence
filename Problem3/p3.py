import math

## Problem A
def problem_a():
  nva = 0
  # part a
  for E in range(1, 29):
    nva += 1
    for F in range(1, 29 - E):
      nva += 1
      if ((((E + F + 21)**2 - 417)/(E*E)).is_integer() and 0 < (((E + F + 21)**2 - 417)/(E*E)) < 51):
        A = int(((E + F + 21)**2 - 417)/(E*E))
        nva += 1
        for B in range(1, A - E - F):
          nva += 1
          C = A - B - E - F
          nva += 1
          D = E + F + 21
          nva += 1
          return (A, B, C, D, E, F, nva)
  return ('None')

## Problem B
def problem_b():
  nva = 0
  # part a
  for E in range(1, 29):
    nva += 1
    for F in range(1, 29 - E):
      nva += 1
      if ((((E + F + 21)**2 - 417)/(E*E)).is_integer() and (((E + F + 21)**2 - 417)/(E*E)) < 51):
        A = int(((E + F + 21)**2 - 417)/(E*E))
        nva += 1
        for B in range(1, A - E - F):
          nva += 1
          C = A - B - E - F
          nva += 1
          D = E + F + 21
          nva += 1
          # part b
          if ((F - math.sqrt(A + D + 1)).is_integer()):
            G = int(F - math.sqrt(A + D + 1))
            nva += 1
            J = int((G**2 + 39)/4)
            nva += 1
            for H in range(1, 51):
              nva += 1
              if ((math.sqrt(H*J + 12*E) - G).is_integer()):
                I = int(math.sqrt(H*J + 12*E) - G)
                nva += 1
                if ((I-G)**9 == (F-H)**3 and (G-C)**2 == F*C*C + 1):
                  return (A, B, C, D, E, F, G, H, I, J, nva)
  return ('None')

# Problem C
def problem_c():
  nva = 0
  # part a
  for E in range(1, 29):
    nva += 1
    for F in range(1, 29 - E):
      nva += 1
      if ((((E + F + 21)**2 - 417)/(E*E)).is_integer() and (((E + F + 21)**2 - 417)/(E*E)) < 51):
        A = int(((E + F + 21)**2 - 417)/(E*E))
        nva += 1
        for B in range(1, A - E - F):
          nva += 1
          C = A - B - E - F
          nva += 1
          D = E + F + 21
          nva += 1
          # part b
          if ((F - math.sqrt(A + D + 1)).is_integer()):
            G = int(F - math.sqrt(A + D + 1))
            nva += 1
            J = int((G**2 + 39)/4)
            nva += 1
            for H in range(1, 51):
              nva += 1
              if ((math.sqrt(H*J + 12*E) - G).is_integer()):
                I = int(math.sqrt(H*J + 12*E) - G)
                nva += 1
                if ((I-G)**9 == (F-H)**3 and (G-C)**2 == F*C*C + 1):
                  # part c
                  if(math.sqrt(G*H*I*B + 133).is_integer()):
                    O = int(math.sqrt(G*H*I*B + 133))
                    nva += 1
                    if ((math.sqrt(14 + 2*O)).is_integer()):
                      K = int(math.sqrt(14 + 2*O))
                      nva += 1
                      M = K**2 - 10 - O
                      nva += 1
                      if((math.sqrt(M**2 + 291)).is_integer() and ((math.sqrt(M**2 + 291)) - O)**3 + 7 == (F - I)*(math.sqrt(M**2 + 291))):
                        N = int(math.sqrt(M**2 + 291))
                        nva += 1
                        for L in range(1, 51):
                          nva += 1
                          if (L**3 + I == (L + B)*K):
                            return (A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, nva)
  return ('None')


# Interface
print('a) Problem A\tb) Problem B\tc) Problem C')
user_input = input('Choose a CSP Problem: ')
if (user_input.lower() == 'a'):
  solution = problem_a()
  if (solution == ('None')):
    print('No Solution Found.')
  else:
    print('Solution for Problem A:')
    print('A = %d, B = %d, C = %d, D = %d, E = %d, F = %d, nva = %d' % solution)
elif (user_input.lower() == 'b'):
  solution = problem_b()
  if (solution == ('None')):
    print('No Solution Found.')
  else:
    print('Solution for Problem B:')
    print('A = %d, B = %d, C = %d, D = %d, E = %d, F = %d, G = %d, H = %d, I = %d, J = %d, nva = %d' % solution)
elif (user_input.lower() == 'c'):
  solution = problem_c()
  if (solution == ('None')):
    print('No Solution Found.')
  else:
    print('Solution for Problem C:')
    print('A = %d, B = %d, C = %d, D = %d, E = %d, F = %d, G = %d, H = %d, I = %d, J = %d, K = %d, L = %d, M = %d, N = %d, O = %d, nva = %d' % solution)
else:
  print('No such option.')