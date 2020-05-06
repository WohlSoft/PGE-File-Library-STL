from comtypes.client import CreateObject

def main():
	pgeFactory = CreateObject("PGE.FileLib")
	newLevelData = pgeFactory.CreateNewLevelData()
	newBlock = newLevelData.Blocks.Add()
	
	print("Num of blocks: " + newLevelData.Blocks.Count)

if __name__ == "__main__":
	main()

