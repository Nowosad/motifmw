#include <comat.h>
#include "RcppArmadillo.h"
// [[Rcpp::depends(comat)]]
using namespace Rcpp;

// [[Rcpp::plugins(cpp11)]]

// [[Rcpp::export]]
int triangular_index(int r, int c) {
  r++;
  c++;
  if (c <= r){
    return (r - 1) * r / 2 + c - 1;
  } else {
    return (c - 1) * c / 2 + r - 1;
  }
}

// [[Rcpp::export]]
NumericMatrix mw_cove_terra(IntegerVector x, NumericMatrix raster_window,
                     arma::imat directions, std::vector<int> classes,
                     bool ordered, std::string normalization,
                     size_t ni, size_t nw) {
    int nrows = raster_window.nrow();
    int ncols = raster_window.ncol();
    int cove_len;
    if (ordered){
      cove_len = classes.size() * classes.size();
    } else {
      cove_len = triangular_index(classes.size() - 1, classes.size() - 1) + 1;
    }
    NumericMatrix out(ni, cove_len);
    std::fill(out.begin(), out.end(), NumericVector::get_na());
    for(size_t i = 0; i < ni; i++) {
      size_t start = i * nw;
      size_t end = start + nw - 1;
      IntegerMatrix values_in_window(nrows, ncols);
      IntegerVector xw = x[Rcpp::Range(start,end)]; //Current window of values
      // do not calculate when focal cell is NA
      if (xw[nw / 2] == NA_INTEGER){
         continue;
      }
      for (int i = 0; i < nrows; i++) {
        for (int j = 0; j < ncols; j++) {
          values_in_window(i, j) = xw[i * ncols + j];
        }
      }
      IntegerMatrix coma = comat::rcpp_get_coma_internal(values_in_window, directions, classes = classes);
      NumericVector cove = comat::rcpp_get_cove(coma, ordered, normalization);
      out(i, _) = cove;
    }
    return(out);
}


/***R
# library(spquery)
# library(motif)
# library(terra)
# lc01 = rast("~/science/moving-window-motif/data/lc_am_1992.tif")
# lc15 = rast("~/science/moving-window-motif/data/lc_am_2018.tif")
# lc01 = crop(lc01, ext(-7e6, -67e5, -13e5, -1e6))
# lc15 = crop(lc15, ext(-7e6, -67e5, -13e5, -1e6))
#
# im = as.matrix(lc01, wide = TRUE)
# im = c(im)
# classes = 1:9
# directions = as.matrix(4)
# window_size0 = 1000
# window0 = matrix(1, nrow = window_size0, ncol = window_size0)
#
# # a = covemw(landscape_values = im, raster_window = window0, directions = directions, classes = classes)
# # str(a)
# #
# # im[2:100] = NA
# # b = covemw(landscape_values = im, raster_window = window0, directions = as.matrix(4), classes = 1:9)
# # b
#
# window_size = 3
# window = matrix(1, nrow = window_size, ncol = window_size)
#
# lc01b = lc01[1:100, 1:100, drop = FALSE]
# lc01b = lc01[1:3, 1:3, drop = FALSE]
# lc01b[2, 2] = 1
# plot(lc01b)
# system.time({
#   lc01sa = terra::focal(x = lc01b, w = dim(window), silent = FALSE, fun = function(x) {
#     # cat(x, "\n")
#     # 1
#     covemw(landscape_values = x,
#            raster_window = window,
#            directions = as.matrix(4),
#            classes = 1:9)
#     # cat(result, "\n")
#     # return(result)
#   })
# })
#
# plot(lc01sa)
#
# calculate_cove_focal = function(landscape_values, raster_window) {
#   extract_cove = function(x, classes){
#     comat::get_cove(comat::get_coma(x, classes = classes), ordered = FALSE, normalization = "pdf")
#   }
#
#   # convert focal window to matrix
#   raster_window[!is.na(raster_window)] <- landscape_values[!is.na(raster_window)]
#
#   result = extract_cove(x = raster_window, classes = 1:9)
#
#   return(result)
# }
#
# system.time({
#   lc01sb = terra::focal(x = lc01b, w = dim(window), silent = FALSE, fun = calculate_cove_focal, raster_window = window)
# })
#
# plot(c(lc01b, lc01sb[[2]], lc01sa[[2]]), nr = 1)
#
# system.time({
#   lc01sc = terra::focalCpp(x = lc01b, w = dim(window),
#                            #silent = FALSE,
#                            fun = covemw_terra, raster_window = window,
#                            directions = as.matrix(4),
#                            classes = 1:9)
# })
#
# plot(c(lc01sa[[1]], lc01sb[[1]], lc01sc[[1]]))
*/
