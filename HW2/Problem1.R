parquad <- function(u, a, val)
{
 require(parallel)
 idx <- splitIndices(nrow(u), myinfo$nwrkrs)[[myinfo$id]]
 rdsmlock("vallock")
 val[1,1] <- val[1,1] + t(u[,]) %*% a[,idx] %*% u[idx,]
 rdsmunlock("vallock")
 0
}
