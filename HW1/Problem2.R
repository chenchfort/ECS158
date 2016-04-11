# Written for ECS 158
#
# Problem 2

library(NMF)     # Required for nmf()

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
  maevec_my <- vector()

  # First, I need to take the kvec and a and use
  # getapprox() to get a vector of approximate values.
  # Note that the closer to 0, the closer to the original
  # image. Thus, the lower the rank the higher the mae.
  for (k in kvec)
  {
    maevec <- append(maevec, mae_my(a,getapprox(a, k)))
    # Backup mae
    #maevec_my <- append(maevec_my, mae_my(a, getapprox(a, k)))
  }

  # Testing
  #print(maevec)
  #print(maevec_my)

  # Now that I have kvec and maevec, I can plot.
  plot(kvec, maevec, type='l', main='MAE vs. Rank',
       xlab='MAE', ylab='Rank')
}

# MAE incase we cant use the one from metrics
mae_my <- function(a, b)
{
  sum <- 0;

  for (r in 1:nrow(a))
    for(c in 1:ncol(b))
      sum = sum + abs(a[r,c] - b[r,c])

  return(sum / (r * c))
}
