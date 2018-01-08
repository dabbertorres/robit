package main

import (
	"text/template"
	"io"
)

type Attribute struct {
	Name  string
	Value string
}

type Element struct {
	Type       string
	Id         string
	Class      []string
	Attributes []Attribute
	Children   []Element
	Content    string
	Empty      bool
}

type PageHead struct {
	Title       string
	Meta        []Attribute
	StyleSheets []string
	Scripts     []string
}

type PageBody struct {
	Header []Element
	Main   []Element
	Footer []Element
}

type Page struct {
	Head PageHead
	Body PageBody
}

var pageTmpl = template.Must(template.New("base").Parse(`<!doctype html>
{{define "element"}}<{{.Type}} {{with .Id}}id="{{.}}"{{end}} {{with .Class}}class="{{range .}}{{.}} {{- end}}"{{end}} {{range .Attributes}}{{.Name}}="{{.Value}}" {{- end}}>{{if not .Empty}}
{{range .Children}}{{template "element" .}}{{else}}{{.Content}}{{end}}
</{{.Type}}>{{end}}{{end}}
<html>
<head>{{with .Head}}
	<title>{{.Title}}</title>
	<meta charset="utf-8">
	{{range .Meta}}<meta {{.Name}}="{{.Value}}">{{end}}
	{{range .StyleSheets}}<link rel="stylesheet" href="{{- . -}}">{{end}}
	{{range .Scripts}}<script>{{.}}</script>{{end}}
{{end}}</head>
<body>{{with .Body}}
	{{with .Header}}<header>{{range .}}{{template "element" .}}{{end}}</header>{{end}}
	{{with .Main}}<main>{{range .}}{{template "element" .}}{{end}}</main>{{end}}
	{{with .Footer}}<footer>{{range .}}{{template "element" .}}{{end}}</footer>{{end}}
{{end}}</body>
</html>`))

func (p *Page) Build(w io.Writer) error {
	return pageTmpl.Execute(w, p)
}
