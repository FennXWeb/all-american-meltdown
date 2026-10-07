"""Offline shoreline routing for the approved, limited story corridor.
Native automation checks these paths against the actual game water function.
"""
import math,heapq
from pathlib import Path
C=((43.0481-42.8864)*1111000,(-76.1474+78.8784)*813000)
def warp(p):
    dx,dy=p[0]-C[0],p[1]-C[1];r=math.hypot(dx,dy)
    f=r*10 if r<=90000 else 900000+(r-90000)*200000/1010000 if r<1100000 else r
    return (C[0]+dx*f/r,C[1]+dy*f/r)
def water(p):
    if 2740000<p[1]<3018000 and abs(p[0]-1270000)<23000:return True
    dx,dy=p[0]-C[0],p[1]-C[1];r=math.hypot(dx,dy)
    f=r/10 if r<=900000 else 90000+(r-900000)*1010000/200000 if r<1100000 else r
    x,y=C[0]+dx*f/r,C[1]+dy*f/r
    west=-140000+240000*min(1,max(0,(x-1000000)/280000))
    return y>west and y<2750000 and x>440000+18000*math.sin(y*.000003) and x<1530000+16000*math.sin(y*.000004)
def clear(a,b):
    n=max(1,math.ceil(math.dist(a,b)/1500))
    return all(not water((a[0]+(b[0]-a[0])*i/n+dx,a[1]+(b[1]-a[1])*i/n+dy)) for i in range(n+1) for dx,dy in [(0,0),(-1800,0),(1800,0),(0,1800),(0,-1800)])
def route(a,b):
    if clear(a,b):return [a,b]
    cell=10000;s=tuple(round(v/cell) for v in a);end=tuple(round(v/cell) for v in b)
    point=lambda n:(n[0]*cell,n[1]*cell)
    q=[(0,s)];g={s:0};prev={};done=set()
    while q:
        _,u=heapq.heappop(q)
        if u in done:continue
        if u==end:break
        done.add(u)
        for dx,dy in [(-1,-1),(-1,0),(-1,1),(0,-1),(0,1),(1,-1),(1,0),(1,1)]:
            v=(u[0]+dx,u[1]+dy)
            if v[0]<10 or v[0]>127 or v[1]<260 or v[1]>350 or not clear(point(u),point(v)):continue
            cost=g[u]+math.hypot(dx,dy)
            if cost<g.get(v,1e30):g[v]=cost;prev[v]=u;heapq.heappush(q,(cost+math.dist(v,end),v))
    assert end in g,(a,b)
    path=[b];u=end
    while u!=s:path.append(point(u));u=prev[u]
    path+=[point(s),a];path.reverse();simple=[path[0]];i=0
    while i<len(path)-1:
        j=len(path)-1
        while j>i+1 and not clear(path[i],path[j]):j-=1
        assert clear(path[i],path[j]);simple.append(path[j]);i=j
    return simple
points=[warp(((43.08-42.8864)*1111000,(-75.99+78.8784)*813000)),(370000,2790000),(550000,2800000),(780000,2850000),(800000,2840000),(875000,3020000),(1100000,2830000),(1190000,2960000),(1240000,2960000)]
segments=[]
for a,b in list(zip(points,points[1:]))+[((875000,3020000),(905000,3210000)),((905000,3210000),(990000,3330000))]:
    p=route(a,b);segments+=list(zip(p,p[1:]))
out=['// Offline shoreline-safe approach segments; Tools/campaign76_roads.py.']
for a,b in segments:out.append('Roads.Add({{%.3f,%.3f},{%.3f,%.3f},780,false});'%(*a,*b))
(Path(__file__).resolve().parents[1]/'Source/LethalWorld/LWCampaign76Roads.inl').write_text('\n'.join(out)+'\n')
print('Dry approach segments:',len(segments))
