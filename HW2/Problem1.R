# ECS 158, Problem1.R
# Computes the quadratic for u'Au
# Uses Rdsm

# parquad(u, a val)
# Input : u - vectors
#         a - Matrix
#         val - return variable
parquad <- function(u, a, val)
{
  require(parallel)

  u_t <- t(u) # transpose u

  index <- splitIndices(nrow(u), myinfo$nwrkrs)[[myinfo$id]]
  val[,index] <- u_t[,index] %*% A[index,]
  val[,] <- val[,index] %*% u[index,]

  return(0)
}
