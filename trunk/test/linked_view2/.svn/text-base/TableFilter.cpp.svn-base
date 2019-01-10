#include <vector>
#include <cassert>
#include <cstring>
#include <kvs/AnyValueArray>
#include <kvs/TableObject>
#include <kvs/KVSMLObjectTable>
#include <TableObjectWriter.h>

#include "TableFilter.h"

namespace tof
{
    TabFilter::TabFilter()
    {
        // null
    }

    void TabFilter::exec(const kvs::TableObject* input, const std::vector<float> lim, const std::string filename)
    {
        size_t nr = input->nrows();
        size_t nc = input->ncolumns();
        assert(lim.size()%2 == 0 && lim.size()/2 == nc);
        kvs::TableObject* output = new kvs::TableObject();
        std::vector<int> index;
        for (size_t i=0; i<nr; i++)
        {
            if (input->column(0).to<float>(i) >= lim[0*2] && input->column(0).to<float>(i) <= lim[0*2+1])
            {
                index.push_back(i);
            }
        }
        for (size_t i=1; i<nc; i++)
        {
            std::vector<float> tmp(index.size());
            std::vector<int> tidx;
            for (size_t j=0; j<index.size(); j++)
            {
                tmp[j] = input->column(i).to<float>(index[j]);
                if (tmp[j] >= lim[i*2] && tmp[j] <= lim[i*2+1])
                {
                    tidx.push_back(index[j]);
                }
            }
            index.clear();
            index = tidx;
        }
        for (size_t i=0; i<nc; i++)
        {
            kvs::ValueArray<float> tmp(index.size());
            for (size_t j=0; j<index.size(); j++)
            {
                tmp[j] = input->column(i).to<float>(index[j]);
            }
            output->addColumn( tmp, input->label(i) );
        }

        kvsoceanvis::util::TableObjectWriter writer( output );
        writer.write(filename);
    }
}
