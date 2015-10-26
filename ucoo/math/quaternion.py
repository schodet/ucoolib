import sympy as sp

sp.init_printing()

def qmul(q1, q2):
    a1, b1, c1, d1 = q1
    a2, b2, c2, d2 = q2
    return (
            a1 * a2 - b1 * b2 - c1 * c2 - d1 * d2,
            a1 * b2 + b1 * a2 + c1 * d2 - d1 * c2,
            a1 * c2 - b1 * d2 + c1 * a2 + d1 * b2,
            a1 * d2 + b1 * c2 - c1 * b2 + d1 * a2
            )

def rotate(q, v):
    qv = (0, v[0], v[1], v[2])
    # q is unitary.
    qinv = (q[0], -q[1], -q[2], -q[3])
    return qmul(q, qmul(qv, qinv))[1:4]

if 0:
    yaw, pitch, roll = sp.symbols('yaw pitch roll')
    cyaw = sp.cos(yaw/2)
    syaw = sp.sin(yaw/2)
    cpitch = sp.cos(pitch/2)
    spitch = sp.sin(pitch/2)
    croll = sp.cos(roll/2)
    sroll = sp.sin(roll/2)
else:
    cyaw, syaw, cpitch, spitch, croll, sroll = \
            sp.symbols('cyaw syaw cpitch spitch croll sroll')

print "yaw, pitch, roll to quaternion"
q1 = (cyaw, 0, 0, syaw)
q2 = (cpitch, 0, spitch, 0)
q3 = (croll, sroll, 0, 0)
qt = qmul(q1, qmul(q2, q3))
for n, l in zip(('w', 'x', 'y', 'z'), qt):
    print '%s = %s;' % (n, l)

print "rotation"
v = sp.symbols('v.x v.y v.z')
q = sp.symbols('w x y z')
rv = rotate(q, v)
for n, l in zip(('x', 'y', 'z'), rv):
    print 'n%s = %s;' % (n, sp.expand(l))
