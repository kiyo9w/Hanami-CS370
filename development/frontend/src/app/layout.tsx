import type { Metadata } from "next";
import { Inter } from "next/font/google";
import "./globals.css";
import Navbar from "@/components/Navbar";
import { EditorProvider } from "@/lib/EditorContext";

const inter = Inter({ subsets: ["latin"] });

export const metadata: Metadata = {
  title: "Hanami Language Playground",
  description: "Interactive playground for the Hanami programming language",
};

export default function RootLayout({
  children,
}: {
  children: React.ReactNode
}) {
  return (
    <html lang="en">
      <body className={inter.className}>
        <EditorProvider>
          <Navbar />
          <main className="container mx-auto px-4 py-8">
            {children}
          </main>
        </EditorProvider>
      </body>
    </html>
  );
}
