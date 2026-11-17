# 기수 정렬(radix sort)
# 공간으로 시간을 버는 전략

BUCKETS=10 #10진수
DIGITS=4   #최대 4자릿수

from queue import Queue          #큐 자료형 사용
def radix_sort(A):
    queues=[]
    for i in range(BUCKETS):     #진수만큼 킷값을 사용하기 위해 배열 준비
        queues.append(Queue())   #각 킷값에 해당하는 자리마다 큐 넣기

    n=len(A)
    factor=1                     #자릿수(1,10,100,1000) 
    for d in range(DIGITS):      #자릿수(0,1,2,3)
        for i in range(n):       #같은 자리값을 비교하기 위해 모든 요소 돌기
            queues[(A[i]//factor)%10].put(A[i])  #각 자릿수의 값에 맞는 킷값의 큐에 put
        j=0
        for b in range(BUCKETS):         #킷값(진수)마다
            while not queues[b].empty(): #각 키의 큐가 비어있는지 확인
                A[j]=queues[b].get()     #비어있지 않은 곳의 큐에서 FIFO로 get
                j+=1
        factor*=10               #각 자릿수를 정렬하면 한 자리 높이기
        print("step", d+1, A)    #단계(자릿수)별 확인

# 시간복잡도: O(dn)=O(n) - line16
# 공간복잡도: O(n)       - line17