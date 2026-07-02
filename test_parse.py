import csv
arr = {}
with open('backend/data/gtfs/Delhi/metro/stop_times.txt', 'r') as f:
    reader = csv.DictReader(f)
    prev_trip = None
    prev_stop = None
    prev_dep = None
    for row in reader:
        trip = row['trip_id']
        stop = row['stop_id']
        
        def parse_time(t):
            h, m, s = map(int, t.split(':'))
            return h*60 + m + s/60.0
            
        arr_t = parse_time(row['arrival_time'])
        dep_t = parse_time(row['departure_time'])
        
        if trip == prev_trip and prev_stop is not None:
            diff = arr_t - prev_dep
            if diff < 0: diff += 24*60
            key = (prev_stop, stop)
            if key not in arr: arr[key] = []
            arr[key].append(diff)
            
        prev_trip = trip
        prev_stop = stop
        prev_dep = dep_t

print(sum(arr[('21', '20')]) / len(arr[('21', '20')]))
print(max(arr[('21', '20')]))
