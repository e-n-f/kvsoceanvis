-- functions used for generating data points
Param.type1 = function (pnum, xmin, xmax, ymin, ymax)
    x = {};
    y = {};
    math.randomseed(os.time());
    math.random(0, 1000);
    for i=1,(pnum/4) do
        x[i] = -(math.random(0, 1000) / 1000 * (xmax - xmin) + xmin);
        y[i] = math.random(0, 1000) / 1000 * (ymax - ymin) + ymin;
    end
    for i=(pnum/4+1),(pnum/4*2) do
        x[i] = -(math.random(0, 1000) / 1000 * (xmax - xmin) + xmin);
        y[i] = -(math.random(0, 1000) / 1000 * (ymax - ymin) + ymin);
    end
    for i=(pnum/4*2+1),(pnum/4*3) do
        x[i] = math.random(0, 1000) / 1000 * (xmax - xmin) + xmin;
        y[i] = -(math.random(0, 1000) / 1000 * (ymax - ymin) + ymin);
    end
    for i=(pnum/4*3+1),pnum do
        x[i] = math.random(0, 1000) / 1000 * (xmax - xmin) + xmin;
        y[i] = math.random(0, 1000) / 1000 * (ymax - ymin) + ymin;
    end
    return x,y;
end

Param.type2 = function (pnum, xmin, xmax, ymin, ymax)
    x = {};
    y = {};
    math.randomseed(os.time());
    math.random(0, 1000);
    for i=1,(pnum/5) do
        x[i] = -(math.random(0, 1000) / 1000 * (xmax - xmin - 1) + xmin + 1);
        y[i] = math.random(0, 1000) / 1000 * (ymax - ymin - 1) + ymin + 1;
    end
    for i=(pnum/5+1),(pnum/5*2) do
        x[i] = -(math.random(0, 1000) / 1000 * (xmax - xmin - 1) + xmin + 1);
        y[i] = -(math.random(0, 1000) / 1000 * (ymax - ymin - 1) + ymin + 1);
    end
    for i=(pnum/5*2+1),(pnum/5*3) do
        x[i] = math.random(0, 1000) / 1000 * (xmax - xmin - 1) + xmin + 1;
        y[i] = -(math.random(0, 1000) / 1000 * (ymax - ymin - 1) + ymin + 1);
    end
    for i=(pnum/5*3+1),(pnum/5*4) do
        x[i] = math.random(0, 1000) / 1000 * (xmax - xmin - 1) + xmin + 1;
        y[i] = math.random(0, 1000) / 1000 * (ymax - ymin - 1) + ymin + 1;
    end
    for i=(pnum/5*4+1),pnum do
        x[i] = -(math.random(0, 1000) / 1000 * (xmax - xmin)) + xmin;
        y[i] = -(math.random(0, 1000) / 1000 * (ymax - ymin - 2)) + ymin - 1;
    end
    return x,y;
end

Param.type3 = function (pnum, xmin, xmax, ymin, ymax)
    x = {};
    y = {};
    math.randomseed(os.time());
    math.random(0, 1000);
    for i=1,(pnum/6) do
        x[i] = -(math.random(0, 1000) / 1000 * (xmax - xmin - 1) + xmin + 1);
        y[i] = math.random(0, 1000) / 1000 * (ymax - ymin - 1) + ymin + 1;
    end
    for i=(pnum/6+1),(pnum/6*2) do
        x[i] = -(math.random(0, 1000) / 1000 * (xmax - xmin - 1) + xmin + 1);
        y[i] = -(math.random(0, 1000) / 1000 * (ymax - ymin - 1) + ymin + 1);
    end
    for i=(pnum/6*2+1),(pnum/6*3) do
        x[i] = math.random(0, 1000) / 1000 * (xmax - xmin - 1) + xmin + 1;
        y[i] = -(math.random(0, 1000) / 1000 * (ymax - ymin - 1) + ymin + 1);
    end
    for i=(pnum/6*3+1),(pnum/6*4) do
        x[i] = math.random(0, 1000) / 1000 * (xmax - xmin - 1) + xmin + 1;
        y[i] = math.random(0, 1000) / 1000 * (ymax - ymin - 1) + ymin + 1;
    end
    for i=(pnum/6*4+1),(pnum/6*5) do
        x[i] = -(math.random(0, 1000) / 1000 * (xmax - xmin)) + xmin;
        y[i] = math.random(0, 1000) / 1000 * (ymax - ymin - 1) + ymin + 1;
    end
    for i=(pnum/6*5+1),pnum do
        x[i] = -(math.random(0, 1000) / 1000 * (xmax - xmin)) + xmin;
        y[i] = -(math.random(0, 1000) / 1000 * (ymax - ymin - 1) + ymin + 1);
    end
    return x,y;
end
