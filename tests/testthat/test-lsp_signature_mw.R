r = terra::rast(matrix(c(rep(1, 4), NA, rep(2, 4)), ncol = 3))
# plot(r)

# square window
sw = matrix(1, 3, 3)

# circle window
cw = terra::focalMat(r, 1, type = "circle")
cw[cw > 0] = 1
cw[cw == 0] = NA

# calc
r_sw = lsp_signature_mw(r, sw)
r_cw = lsp_signature_mw(r, cw)

test_that("square window works", {
  interactions_12 = terra::as.data.frame(r_sw)[[2]]
  expect_equal(c(0, 0.25, 0.5, 0.25, 0.25, 0.5, 0.25, 0), interactions_12)
  expect_equal(c(1, 1, 1), terra::global(r_sw, fun = "isNA")[[1]])
})

test_that("circle window works", {
  interactions_12 = terra::as.data.frame(r_cw)[[2]]
  expect_equal(c(0, 0.5, 0.5, 0, 0, 0.5, 0.5, 0), interactions_12)
  expect_equal(c(1, 1, 1), terra::global(r_cw, fun = "isNA")[[1]])
})
