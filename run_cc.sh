TEST="1"
DATA="/data/gunrock_dataset/large"
DATA2="/data/topc-datasets"

for file in ak2010 belgium_osm coAuthorsDBLP delaunay_n10 delaunay_n13 delaunay_n21 webbase-1M soc-LiveJournal1 kron_g500-logn21
do
  if [ "$TEST" = "0" ] ; then
    echo bin/gcc --mxvmode=0 --timing 0 --directed=2 --niter=5 $DATA/$file/$file.mtx
    bin/gcc --mxvmode=0 --timing 0 --directed=2 --niter=5 $DATA/$file/$file.mtx
  fi
done

for file in soc-orkut soc-LiveJournal1 hollywood-2009 indochina-2004 rmat_n22_e64 rmat_n23_e32 rmat_n24_e16 rgg_n24_0.000548 road_usa
do
  if [ "$TEST" = "1" ] ; then
    echo bin/gcc --mxvmode=0 --timing 0 --directed=2 --niter=5 $DATA2/$file.mtx
    bin/gcc --mxvmode=0 --timing 0 --directed=2 --niter=5 $DATA2/$file.mtx
  fi
done

for file in test_bc test_cc test_mesh test_mis test_pr small chesapeake
do
  if [ "$TEST" = "2" ] ; then
    echo bin/gcc --mxvmode=0 --timing 0 --directed=2 --niter=5 data/small/$file.mtx
    bin/gcc --mxvmode=0 --timing 0 --directed=2 --niter=5 data/small/$file.mtx
  fi
done
