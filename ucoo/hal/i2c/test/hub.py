# Hub for test.
from mex.hub import Hub
def log (x):
    print x
h = Hub (min_clients = 1, log = log)
h.wait ()
