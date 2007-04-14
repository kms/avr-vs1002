#!/usr/bin/ruby1.8

b = Array.new
bs = 256

f = File.open(ARGV[0])

t1 = Time.now
t2 = Time.now

while true
    begin
	b.push(f.read(bs))
	if (STDIN.read_nonblock(1) == '.')
	    d = t1 - t2

	    STDERR.write("%.2fkb/s [%dB]\n" % [((bs * 8) / d / 1000), b.length])
	    puts (t2 - t1).to_s
	    t2 = t1
	    t1 = Time.now

	    print b.shift
	    STDOUT.flush
	end
    rescue Errno::EAGAIN => e
    end
end
