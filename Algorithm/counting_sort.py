# 카운팅 정렬(counting sort)
# 공간으로 시간을 버는 전략

MAX_VAL=50

def counting_sort(A):
    output=[0]*len(A) #
    count=[0]*MAX_VAL #

    for i in A:
        count[i]+=1   #

    for i in range(MAX_VAL): #
        count[i]+=count[i-1] #

    for i in range(len(A)):
        output[count[A[i]-1]]=A[i] #
        count[A[i]]-=1             #

    for i in range(len(A)):
        A[i]=output[i]             #