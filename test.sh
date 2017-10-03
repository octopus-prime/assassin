docker rm -f assassin &>/dev/null || true
docker run -d -it --name assassin --privileged octopusprime/test
docker exec assassin mkdir assassin
docker cp . assassin:/assassin
docker exec assassin bash -c "
	cd assassin; 
	bjam toolset=$1 debug -j$2
"
docker cp assassin:/assassin/bin .
