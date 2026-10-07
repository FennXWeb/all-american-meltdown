import urllib.request,pathlib,json,time
p=pathlib.Path('ContentSource/Geography84');p.mkdir(parents=True,exist_ok=True)
for name,url in [('natural_roads.json','https://raw.githubusercontent.com/nvkelso/natural-earth-vector/master/geojson/ne_10m_roads.geojson'),('countries.json','https://raw.githubusercontent.com/nvkelso/natural-earth-vector/master/geojson/ne_10m_admin_0_countries.geojson'),('natural_lakes.json','https://raw.githubusercontent.com/nvkelso/natural-earth-vector/master/geojson/ne_10m_lakes.geojson'),('natural_rivers.json','https://raw.githubusercontent.com/nvkelso/natural-earth-vector/master/geojson/ne_10m_rivers_lake_centerlines.geojson'),('syr_airport.osm','https://api.openstreetmap.org/api/0.6/map?bbox=-76.125,43.105,-76.07,43.123')]:
 if p.joinpath(name).exists():continue
 try:
  d=urllib.request.urlopen(url,timeout=50).read();p.joinpath(name).write_bytes(d);print(name,len(d),flush=True)
 except Exception as e:print(name,str(e),flush=True)
places=[('rochester',43.1566,-77.6088),('batavia',42.9981,-78.1875),('canandaigua',42.886,-77.281),('geneva',42.869,-76.9777),('auburn',42.9317,-76.5661),('seneca_falls',42.9106,-76.7966),('newark',43.0468,-77.0953),('palmyra',43.0639,-77.2333),('medina',43.2201,-78.3869),('lockport',43.1706,-78.6903),('rome',43.2128,-75.4557),('camden',43.3348,-75.7474),('pulaski',43.567,-76.1277),('watertown',43.9748,-75.9108),('clayton',44.2395,-76.0858),('alexandria_bay',44.3359,-75.9177),('old_forge',43.7101,-74.9743),('lowville',43.7867,-75.4919),('toronto',43.665,-79.39)]
for name,lat,lon in places:
 f=p/(name+'.osm')
 if f.exists():continue
 try:
  url=f'https://api.openstreetmap.org/api/0.6/map?bbox={lon-.012},{lat-.009},{lon+.012},{lat+.009}'
  d=urllib.request.urlopen(url,timeout=35).read();f.write_bytes(d);print(name,len(d),flush=True);time.sleep(.7)
 except Exception as e:print(name,str(e),flush=True)
