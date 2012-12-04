# Hub for test.
from mex.hub import Hub
def log (x):
    print x
h = Hub (min_clients = 2, log = log)
h.wait ()
