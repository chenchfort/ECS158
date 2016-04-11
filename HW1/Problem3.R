library(NMF)
library(snow)
library(parallel)

plottimes <- function(cls,a,k,clssizevec)
{
  timevec <- vector()
  
  for (n in clssizevec)
  {
    timevec <- append(timevec, system.time(nmfsnow(n, a, k)))
  }
  
  plot(clssizevec, timevec, type='l', main='Node vs. Time',
       xlab='Node', ylab='Time')
}
