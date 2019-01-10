Param = {}; -- program parameters
Param.k = 10; -- number of clusters
Param.n = 80; -- number of points
Param.show = true; -- show input data as scatter plot
-- parameters of generated x and y
Param.xmin = 2;
Param.ymin = 2;
Param.xmax = 6;
Param.ymax = 6;
-- functions used for generating data points
require("function");
-- generate x and y
Param.x = {};
Param.y = {};
Param.x, Param.y = Param.type1(Param.n, Param.xmin, Param.xmax, Param.ymin, Param.ymax);
