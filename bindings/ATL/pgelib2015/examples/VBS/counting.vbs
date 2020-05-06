Set PGEObject = CreateObject("PGE.FileLib")
Set newLevelData = PGEObject.CreateNewLevelData()
Set newBlock = newLevelData.Blocks.Add()


MsgBox("Number of Blocks: " & newLevelData.Blocks.Count)

