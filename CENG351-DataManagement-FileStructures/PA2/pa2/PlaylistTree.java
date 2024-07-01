import java.util.ArrayList;

public class PlaylistTree {
	
	public PlaylistNode primaryRoot;		//root of the primary B+ tree
	public PlaylistNode secondaryRoot;	//root of the secondary B+ tree
	public PlaylistTree(Integer order) {
		PlaylistNode.order = order;
		primaryRoot = new PlaylistNodePrimaryLeaf(null);
		primaryRoot.level = 0;
		secondaryRoot = new PlaylistNodeSecondaryLeaf(null);
		secondaryRoot.level = 0;
	}
	public boolean inttobool(int a)  //toconvert an integer value to a boolean value.
	{
		boolean boolValue;
		if (a<0) {
			boolValue = true;
		}
		else {
			boolValue = false;
		}
		return boolValue;
	}
	public void addSong(CengSong song) {
		// TODO: Implement this method
		// add methods to fill both primary and secondary tree
		// add to primary tree
		//if primary root->leaf
		if(primaryRoot.getType() == PlaylistNodeType.Leaf)
		{
			ArrayList<CengSong> songs;
			Integer length, i;
			//if there are space
			if(((PlaylistNodePrimaryLeaf)primaryRoot).songCount() < 2*PlaylistNode.order)
			{
				songs = ((PlaylistNodePrimaryLeaf)primaryRoot).getSongs();
				length = songs.size();
				for(i = 0; i<length && songs.get(i).audioId() < song.audioId(); i++);
				((PlaylistNodePrimaryLeaf)primaryRoot).addSong(i, song);

			}
			//if it's full

			else
			{
				ArrayList<Integer> ids;
				ArrayList<PlaylistNode> children;
				PlaylistNodePrimaryLeaf new1, new2;
				Integer nodeid;
				songs = ((PlaylistNodePrimaryLeaf)primaryRoot).getSongs();
				length = songs.size();
				for(i = 0; i<length && songs.get(i).audioId()<song.audioId(); i++);
				songs.add(i, song);
				nodeid = songs.get(PlaylistNode.order).audioId();
				ids = new ArrayList<>();
				ids.add(nodeid);
				children = new ArrayList<>();
				new1 = new PlaylistNodePrimaryLeaf(null, (new ArrayList<>(songs.subList(0, PlaylistNode.order))));
				new2 = new PlaylistNodePrimaryLeaf(null, (new ArrayList<>(songs.subList(PlaylistNode.order, 2*PlaylistNode.order+1))));
				children.add(new1);
				children.add(new2);
				primaryRoot = new PlaylistNodePrimaryIndex(null, ids, children);
				new1.setParent(primaryRoot);
				new2.setParent(primaryRoot);

			}

		}
		//if node
		else
		{
			((PlaylistNodePrimaryIndex)primaryRoot).addSong(song);
			if(primaryRoot.getParent()!=null)
			{
				primaryRoot = primaryRoot.getParent();
			}
		}
		if (secondaryRoot.getType() == PlaylistNodeType.Leaf)
		{
			ArrayList<ArrayList<CengSong>> songBucket;
			Integer length, i;
			// if it is not full
			if (((PlaylistNodeSecondaryLeaf)secondaryRoot).genreCount() < 2*PlaylistNode.order){
				PlaylistNodeSecondaryLeaf lef = ((PlaylistNodeSecondaryLeaf)secondaryRoot);
				length = lef.genreCount();
				for(i=0; i<length && inttobool(((lef.genreAtIndex(i)).compareTo(song.genre()))); i++);//songBucket.get(i).get(0).genre() strcompare?
				if(i>0 && i<length && (lef.genreAtIndex(i)).equals(song.genre()))
				{
					((PlaylistNodeSecondaryLeaf)secondaryRoot).addSong(i,song);

					return; //equals for string equality check! if equals, don't add to tree.
				}
				((PlaylistNodeSecondaryLeaf)secondaryRoot).addSong(i,song);
			}
			// full -> split leaf and make the one in middle the parent
			else{
				ArrayList<String> genres;
				ArrayList<PlaylistNode> children;
				PlaylistNodeSecondaryLeaf newLeaf1, newLeaf2;
				String nodeGenre;
				PlaylistNodeSecondaryLeaf songs = ((PlaylistNodeSecondaryLeaf)secondaryRoot);
				length = songs.genreCount();  //(s1.compareTo(s3));  1(because s1>s3)
				for(i=0; i<length && inttobool(((songs.genreAtIndex(i)).compareTo(song.genre()))) ; i++); //songs.genreAtIndex(i) string compare etme methodu bulmak lazim. ((song.genre()).compareTo(songs.get(i).genre()))
				if(i<length && (songs.genreAtIndex(i)).equals(song.genre()))
				{
					songs.addSong(i,song);
					return;
				}
				songs.addSong(i,song);
				nodeGenre = songs.genreAtIndex(PlaylistNode.order);
				genres = new ArrayList<>();
				genres.add(nodeGenre);
				children = new ArrayList<>();
				newLeaf1 = new PlaylistNodeSecondaryLeaf(null, (new ArrayList<> (songs.getSongBucket().subList(0,PlaylistNode.order))));
				newLeaf2 = new PlaylistNodeSecondaryLeaf(null, (new ArrayList<> (songs.getSongBucket().subList(PlaylistNode.order,2*PlaylistNode.order+1))));

				children.add(newLeaf1);
				children.add(newLeaf2);
				secondaryRoot = new PlaylistNodeSecondaryIndex(null, genres, children); //parent,genres,children
				newLeaf1.setParent(secondaryRoot);
				newLeaf2.setParent(secondaryRoot);
			}
		}
		//node
		else{
			((PlaylistNodeSecondaryIndex)secondaryRoot).addSong(song); //playlistnodesecondaryindex'e addSong methodu eklenecek!!
			if(secondaryRoot.getParent()!=null){
				secondaryRoot = secondaryRoot.getParent();
			}
		}

		return;
	}
	
	public CengSong searchSong(Integer audioId) {
		// TODO: Implement this method
		// find the song with the searched audioId in primary B+ tree
		// return value will not be tested, just print according to the specifications
		//only leaves exist:
		if (primaryRoot.getType() == PlaylistNodeType.Leaf){
			ArrayList<CengSong> songs;
			Integer length, i;
			songs = ((PlaylistNodePrimaryLeaf)primaryRoot).getSongs();
			length = songs.size();
			for(i=0; i<length && songs.get(i).audioId()<audioId; i++);
			if(songs.get(i).audioId()==audioId){
				System.out.println("<data>");
				System.out.print("\t");
				System.out.println("<record>" + songs.get(i).audioId() + "|" + songs.get(i).genre() + "|" + songs.get(i).songName() + "|" + songs.get(i).artist() + "</record>");
				System.out.println("</data>");
				return songs.get(i);
			}
			//no record found in leaves
			else{
				System.out.print("Could not find ");
				System.out.println(audioId);
				System.out.println(".");
				return null;
			}
		}
		//node -> call searchSong in PlaylistNodePrimayIndex
		else{
			return ((PlaylistNodePrimaryIndex)primaryRoot).searchSong(audioId, 0);
		}
	}
	
	
	public void printPrimaryPlaylist() {
		// TODO: Implement this method
		// print the primary B+ tree in Depth-first order
		// if it is leaf
		if (primaryRoot.getType() == PlaylistNodeType.Leaf){
			ArrayList<CengSong> songs;
			Integer length, i;
			songs = ((PlaylistNodePrimaryLeaf)primaryRoot).getSongs();
			length = songs.size();
			//only leaves will be printed in data/data
			System.out.println("<data>");
			for(i=0; i<length; i++){
				System.out.println("<record>" + songs.get(i).audioId() + "|" + songs.get(i).genre() + "|" + songs.get(i).songName() + "|" + songs.get(i).artist() + "</record>");
			}
			System.out.println("</data>");
		}
		//node ->call print in PlaylistPrimaryIndex
		else{
			((PlaylistNodePrimaryIndex)primaryRoot).print(0);
		}

		return;
	}
	
	public void printSecondaryPlaylist() {
		// TODO: Implement this method
		// print the secondary B+ tree in Depth-first order
		if (secondaryRoot.getType() == PlaylistNodeType.Leaf){
			ArrayList<ArrayList<CengSong>> songs;
			PlaylistNodeSecondaryLeaf curr;
			Integer length, i;
			curr = ((PlaylistNodeSecondaryLeaf)secondaryRoot);
			songs = ((PlaylistNodeSecondaryLeaf)secondaryRoot).getSongBucket();
			length = songs.size();
			System.out.println("<data>");
			for(i=0; i<length; i++)
			{
				System.out.println(curr.genreAtIndex(i));
				ArrayList<CengSong> sons = curr.songsAtIndex(i);
				int lens = sons.size();
				for(int j = 0; j<lens; j++)
				{
					System.out.print("\t");
					System.out.println("<record>" + sons.get(j).audioId() + "|" + sons.get(j).genre() + "|" + sons.get(j).songName() + "|" + sons.get(j).artist() + "</record>");
				}
			}
			System.out.println("</data>");
		}
		//node->call the print in PlaylistSecondaryIndex
		else{
			((PlaylistNodeSecondaryIndex)secondaryRoot).print(0);
		}
		return;
	}
	
	// Extra functions if needed

}
