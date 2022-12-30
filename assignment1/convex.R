#! /usr/bin/env Rscript
png("convex.png", width=700, height=700)
plot(1:10000, 1:10000, type="n")

#points
points(7925,5297)
points(3130,6293)
points(5233,2568)
points(6032,3568)
points(9795,215)
points(8000,3960)
points(6388,629)
points(515,4463)
points(9613,8571)
points(4064,489)

#line segments
segments(3130,6293,515,4463)
segments(3130,6293,9613,8571)
segments(9795,215,9613,8571)
segments(9795,215,4064,489)
segments(515,4463,4064,489)
dev.off()
