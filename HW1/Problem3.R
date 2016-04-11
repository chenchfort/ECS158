library(NMF)
library(snow)
library(parallel)

plottimes <- function(cls,a,k,clssizevec)
{
  timevec <- vector()
  
  for (n in clssizevec)
  {
    timevec <- append(timevec, system.time(nmfsnow(n, a, k))[[3]])
  }
  
  plot(clssizevec, timevec, type='l', main='Node vs. Time',
       xlab='Node', ylab='Time')
}

nmfsnow <- function(cls,a,k)
{
  rowgrps <- splitIndices(nrow(a), length(cls))
  chunks <- Map(function(grp) a[grp,], rowgrps)
  rowapprox <-function(m,k)
  {
    library(NMF)
    pixmapGrey <- nmf(m, k)
    w <- pixmapGrey@fit@W
    h <- pixmapGrey@fit@H
    approx <- w %*% h
    return(approx)
  }
  mout <- clusterApply(cls, chunks, rowapprox, k)
  c <- mout[[1]]
  for (i in 2:length(mout))
    c <- rbind(c, mout[[i]])
  c <- pmin(c, 1)
  return(c)
}
