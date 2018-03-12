import subprocess

from matplotlib import pyplot


def getParallelTime(file_name, input_file, num_threads):
    with open(file_name, 'r') as file:
        data = file.readlines()

    data[16] = '#define NUMTHREADS {}\n'.format(num_threads)

    with open(file_name, 'w') as file:
        file.writelines(data)

    p = subprocess.Popen(['gcc', file_name, '-fopenmp','-lm'])
    p.communicate()
    input_stream = open(input_file)
    p = subprocess.Popen(['./a.out',input_file],
                         stdin=input_stream,
                         stdout=subprocess.PIPE,
                         stderr=subprocess.PIPE)
    output, error = p.communicate()
    input_stream.close()
    return float(output.decode('utf-8')[:-1])


def getSeqTime(file_name, input_file):
    p = subprocess.Popen(['gcc', file_name,'-lm'])
    p.communicate()
    input_stream = open(input_file)
    p = subprocess.Popen(['./a.out',input_file],
                         stdin=input_stream,
                         stdout=subprocess.PIPE,
                         stderr=subprocess.PIPE)
    output, error = p.communicate()
    input_stream.close()
    print(output)#.decode('utf-8')[:-1])
    return float(output.decode('utf-8')[:-1])


def saveGraph(file_name, input_file):
    nthreads = [1, 2, 4, 8, 16, 32]
    serial = getSeqTime(file_name, input_file)
    parallel_time = [getParallelTime(file_name, input_file, x) for x in nthreads]
    save_name = input_file.split('/')[-1].split('.')[0]
    pyplot.title(save_name)
    pyplot.xlabel("Number of threads")
    pyplot.ylabel("Time (seconds)")
    pyplot.plot(nthreads, parallel_time, 'purple')
    pyplot.plot((1, 32), (serial, serial), 'black')
    pyplot.grid(True)
    pyplot.savefig('graphs/{}.png'.format(save_name), bbox_inches='tight')
    pyplot.clf()


if __name__ == '__main__':
    files = ['input/asteroids1000.txt', 'input/cluster2582.txt',
             'input/collision1.txt', 'input/collision2.txt',
             'input/galaxy1.txt', 'input/galaxy2.txt', 'input/galaxy3.txt',
             'input/galaxy4.txt', 'input/galaxy10k.txt',
             'input/galaxy20k.txt', 'input/galaxy30k.txt',
             'input/galaxyform2500.txt', 'input/galaxymerge1.txt',
             'input/galaxymerge2.txt', 'input/galaxymerge3.txt',
             'input/planets.txt', 'input/saturnrings.txt',
             'input/spiralgalaxy.txt']

    for file in files:
        saveGraph('BarnesHut_OpenMP_V2.c', file)
