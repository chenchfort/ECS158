
nmfsnow <- function(cls,a,k)
{
  rowgrps <- splitIndices(nrow(a), length(c2))
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
  mout <- clusterApply(c2, chunks, rowapprox, k)
  c <- mout[[1]]
  for (i in 2:length(mout))
    c <- rbind(c, mout[[i]])
  c <- pmin(c, 1)
  return(c)
}

plottimes <- function(cls,a,k,clssizevec)
{


}
