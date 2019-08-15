#pragma once

struct FileServices
{
	static void copyFiles ( const juce::String & searchFolder, const juce::String & destFolder, const juce::String & wildcards, bool recursive = true )
	{
		juce::DirectoryIterator iter (juce::File (searchFolder), recursive, wildcards.getCharPointer().getAddress());

		while (iter.next())
		{
			juce::File file (iter.getFile());

			juce::String dest = destFolder + "/" + file.getFileName();
			file.copyFileTo(dest);
		}
	}

	static void moveFiles ( const juce::String & searchFolder, const juce::String & destFolder, const juce::String & wildcards )
	{
		juce::DirectoryIterator iter (juce::File (searchFolder), true, wildcards.getCharPointer().getAddress());

		while (iter.next())
		{
			juce::File file (iter.getFile());

			juce::String dest = destFolder + "/" + file.getFileName();
			file.moveFileTo(dest);
		}
	}

	static void getFiles ( const juce::String & searchFolder, juce::StringArray & filesToGet, const juce::String & wildcards, bool recursive = true )
	{
		juce::DirectoryIterator iter (juce::File (searchFolder), recursive, wildcards.getCharPointer().getAddress());

		while (iter.next())
		{
			juce::File file (iter.getFile());
			filesToGet.add( file.getFullPathName() );
		}
	}

	static void getFilesAndFolders(const juce::String & searchFolder, juce::StringArray & filesToGet, const juce::String & wildcards, bool recursive = true)
	{
		juce::DirectoryIterator iter(juce::File(searchFolder), recursive, wildcards.getCharPointer().getAddress(), juce::File::findFilesAndDirectories);

		while (iter.next())
		{
			juce::File file(iter.getFile());
			filesToGet.add(file.getFullPathName());
		}
	}

	static void getFolders(const juce::String & searchFolder, juce::StringArray & foldersToGet, bool recursive = true)
	{
		juce::DirectoryIterator iter(juce::File(searchFolder), recursive, "*", juce::File::findDirectories);

		while (iter.next())
		{
			juce::File file(iter.getFile());
			foldersToGet.add(file.getFullPathName());
		}
	}

	static void copyFolder ( const juce::String & folderToCopy, const juce::String & destFolder, const juce::String & wildcards )
	{
		juce::DirectoryIterator iter (juce::File (folderToCopy), true, wildcards.getCharPointer().getAddress());

		while (iter.next())
		{
			juce::File file (iter.getFile());

			juce::String dest = destFolder + "/" + file.getFileName();
			file.copyFileTo(dest);
		}
	}

	static void getTextFileLines ( const juce::String & filePath, juce::StringArray & lines )
	{
		juce::File file(filePath);
		if( !file.exists() )
			return;
		else
			file.readLines(lines);
	}

	static juce::String findFilePath ( const juce::String & searchFolder, const juce::String & fileName)
	{
		juce::DirectoryIterator iter (juce::File (searchFolder), true, "*");

		while (iter.next())
		{
			juce::File file (iter.getFile());
			if( file.getFileName() == fileName )
				return file.getFullPathName();
		}

		return juce::String();
	}
};