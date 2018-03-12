from matplotlib import pyplot

'''
input:
filename
serial_time
parallel_times for [1, 2, 4, 8, 16, 32] threads; space seperated
'''

filename = raw_input()
serial = float(raw_input())
parallel = raw_input().split()
parallel = [float(x) for x in parallel]
threads = [1, 2, 4, 8, 16, 32]

pyplot.title(filename)
pyplot.xlabel("Number of threads")
pyplot.ylabel("Time (seconds)")
pyplot.plot(threads, parallel, 'b')
pyplot.plot((1, 32), (serial, serial), 'g')
pyplot.grid(True)
pyplot.show()
