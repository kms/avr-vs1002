#!/usr/bin/ruby1.8
# avr-vs1002
# Streaming from serial port to VS1002.
#
# (c) Karl-Martin Skontorp <kms@skontorp.net> ~ http://22pf.org/
# Licensed under the GNU GPL 2.0 or later.

b = Array.new
bs = 1000

f = File.open(ARGV[0])

t1 = Time.now
t2 = Time.now

while true
    begin
        r = f.read(bs)
        if (r != nil) 
            b.push(r)
        end
        if (STDIN.read_nonblock(1) == '.')
            d = t1 - t2

            STDERR.write("%.2fkb/s [%dB]\n" % [((bs * 8) / d / 1000), b.length * bs])

            print b.shift
            STDOUT.flush

            t2 = t1
            t1 = Time.now
        end
    rescue Errno::EAGAIN => e
    end
end
