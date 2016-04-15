# ECS 158, Problem1.R
# Computes the quadratic for u'Au
# Uses Rdsm

# parquad(u, a val)
# Input : u - VECTOR BUILT FOR MATRIX
#         a - Matrix
#         val - return variable
parquad <- function(u, a, val)
{
  require(parallel)
  ut <- t(u) # transpose u

  # Replace with splitIndices(nrow(u), myinfo$nwrkrs)[[myinfo$id]]
  index <- splitIndices(nrow(u), 2)[[2]]

  val[index,] <- ut[,index] %*% a[index,]
  val <- val[index,] %*% u[,]

  0
}
