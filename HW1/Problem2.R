# Written for ECS 158
#
# Problem 2

library(NMF)     # Required for nmf()
library(Metrics) # Required for mae()

# getapprox(a, k)
# Input : a - matrix from the pixlemap
#         k - rank of matrix
getapprox <- function(a,k)
{
  pixmapGrey <- nmf(a, k)
  w <- pixmapGrey@fit@W
  h <- pixmapGrey@fit@H

  approx <- w %*% h
  approx <- pmin(approx, 1)

  return(approx)
}

# plotmae(a, kvec)
# Input : a    - matrix of pixlemap
#         kvec - vector of ranks
plotmae <- function(a, kvec)
{
  # Set a vector of values from mae to empty
  maevec <- vector()

  # First, I need to take the kvec and a and use
  # getapprox() to get a vector of approximate values.
  # Note that the closer to 0, the closer to the original
  # image. Thus, the lower the rank the higher the mae.
  for (k in kvec)
    maevec <- append(maevec, mae(a,getapprox(a, k)))
  print(kvec)

  # Now that I have kvec and maevec, I can plot.
  plot(kvec, maevec, type='l', col='r', main='MAE vs. Rank',
       xlab='MAE', ylab='Rank')
}
